// count primes in a given interval in parallel

#include <pthread.h>
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// uint4 - unsigned 4-byte int
// Shorter to type than "unsigned int"
typedef unsigned int uint4;

pthread_mutex_t largestPrimeMutex = PTHREAD_MUTEX_INITIALIZER;
uint4 largestPrime = 0;

// a Job is a struct that represents the work that a thread
// needs to do.  In this case, it must describe what integers
// this thread needs to check.  It will also include the
// result - the number of primes that the thread found.
struct Job
{
  // We will divide the range [a,b] into a large number of slices,
  // and each job will handle several slices.
  // see num_primes() for more details.
  //
  uint4 start;       // Start of this job's first slice
  uint4 end;         // Every job has the same end: b+1
  uint4 sliceLength; // Length of each slice
  uint4 sliceStep;   // When a slice is done, jump forward sliceStep = tn * sliceLength
  uint4 res;         // Store result here: sum of primes in all slices this thread examined
};


// return true iff x is a prime number
bool is_prime(uint4 x)
{
  if( x == 2 ) {
    return true;
  }
  if(( x < 2 ) || !( x & 1 )) {
    // x is < 2 or divisible by 2
    // (because its least-significant bit is 0)
    return false;
  }
  if( x <= 7 ) {
    // After the above checks, this is just
    // 3, 5, 7 -- all prime.
    return true;
  }

  int root = (int)sqrt(x) + 1; // Largest number that could divide x

  // Check all odd numbers between 3 and root, inclusive,
  // to see if x is divisible by that number
  int d = 3;
  while( ( d <= root ) && (x % d != 0 ) ) {
    d += 2;
  }

  // If d <= root, then we found a divisor (loop exited
  // because of second term)
  // If d > root, then we didn't find a divisor (loop
  // exited because of first term)
  return d > root;
}



// All threads will run this function when started.
// The thread retrieves the job that was passed in as an
// argument, checks for prime numbers, and then saves
// its answer back into the job struct.
void *prime_thread(void *data)
{
  Job *job = (Job*)data;
  job->res = 0;
  uint4 slice;
  uint4 sliceEnd;
  uint4 i;

  for( slice = job->start; slice < job->end; slice += job->sliceStep ) {
    sliceEnd = slice + job->sliceLength;
    if( sliceEnd > job->end ) {
      sliceEnd = job->end;
    }
    for( i = slice; i < sliceEnd; i++ ) {
      if( is_prime( i ) ) {
	job->res += 1;
	if( i > largestPrime) {
	  pthread_mutex_lock( &largestPrimeMutex );
	  // It is not a mistake to have this check twice.
	  // If we only did the first check, it's possible that
	  // another thread could change largestPrime after we
	  // do the first check, but before we get the mutex.
	  // We would then take the mutex and overwrite the
	  // other thread's largestPrime, possibly incorrectly.
	  // Another solution would just be to have one check
	  // and the mutex outside of it, but then we would
	  // unnecessarily wait for the mutex every time, even
	  // if i is not the largest prime.
	  if( i > largestPrime) {
	    largestPrime = i;
	  }
	  pthread_mutex_unlock( &largestPrimeMutex );
	}
      }
    }
  }

  return 0;
}

// compute number of primes in interval [a, b] using tn pthreads
uint4 num_primes(uint4 a, uint4 b, int tn)
{
  assert(a <= b);
  assert(tn > 0);
  pthread_t threads[tn];
  Job jobs[tn];

  const int sliceLength = 250;
  const int sliceStep = sliceLength * tn;

  // create work items and launch threads.
  for (int i=0; i < tn; ++i) {

    // There are several ways we could divide the range [a,b]
    // between threads.  To make the program as fast as possible,
    // we want every thread to have an approximately equal amount
    // of work.  That keeps the CPUs busy, instead of having some
    // threads finish fast and leave some CPUs idle.
    //
    // Very simple approach: have each thread handle one range,
    // of width w=((b-a)+1) / tn
    //  - thread 0 handles [a..w)
    //  - thread 1 handles [w..2w)
    //  - thread k handles [kw..b+1)
    // This has a problem: smaller numbers are easier to check 
    // than larger ones, because they have fewer possible divisors.
    // So the first threads would finish much faster than later ones,
    // and so we'd be unbalanced.
    //
    // Slightly fancier approach:
    //  - thread 0 handles a+0, a+0+tn, a+0+2tn, a+0+3tn, ...
    //  - thread 1 handles a+1, a+1+tn, a+1+2tn, a+1+3tn, ...
    //  - thread 2 handles a+2, a+2+tn, a+2+2tn, a+2+3tn, ...
    // Now each thread has a mix of small numbers and big numbers.
    // But if tn is even, then every second thread is trivial - all
    // even numbers!  Same with tn divisible by 3 and every third thread,
    // etc.
    //
    // This approach:
    // Divide [a,b] into a large number of slices of equal width,
    // (sliceLength = 250 in our code above).  Each thread will
    // handle a number of slices spread out across the range [a,b].
    // Some slices are easier than others (ie, earlier is easier
    // than later).  But every slice some even numbers, some
    // numbers divisible by 3, etc.  So no thread should have a much
    // easier job than another.

    jobs[i].start = sliceLength * i;
    jobs[i].sliceLength = sliceLength;
    jobs[i].sliceStep = sliceStep;
    jobs[i].end = b+1; // [start,end) == [start,b]
    pthread_create( &threads[ i ], 0, prime_thread, &jobs[i] );
  }


  // wait for threads to complete 
  for( int i = 0; i < tn; ++i ) {
    pthread_join( threads[ i ], NULL );
  }  


  // add up partial sums computed by the jobs
  uint4 primes = 0;
  for (int i=0; i < tn; ++i) {
    primes += jobs[ i ].res;
  }
  return primes;
}


int main(int argc, char *argv[])
{
  if (argc != 4) {
    printf("usage: %s a b tn\nComputes the number of primes in [a,b] using tn threads\n", argv[0]);
    exit(1);
  }

#if 0
  // test is_prime
  for (int i=0; i < 1000; ++i) {
    printf("%6d %d\n", i, is_prime(i));
  }
#endif
  
  uint4 a = 1;
  uint4 b = 1000;
  int   tn = 1;

  a = atoi(argv[1]);
  b = atoi(argv[2]);
  tn = atoi(argv[3]);

  assert(a >= 1);
  assert(a <= b);
  assert(tn > 0);

  printf("a=%u b=%d tn=%d\n", a, b, tn);

  int result = num_primes(a, b, tn);
  printf("there are %d primes in [%u,%u]\n", result, a, b);
  printf("largest prime found: %u\n", largestPrime );

  return 0;
}

/*
  compile with

    g++ -O3 -o primes primes.c -lpthread
    

  runtimes and speedup compared to tn=1:

                            runtime   speedup 
  ./primes 1 50000000 1                  1 
  ./primes 1 50000000 2
  ./primes 1 50000000 3
  ./primes 1 50000000 4
  ./primes 1 50000000 5
  ./primes 1 50000000 6
  ./primes 1 50000000 7
  ./primes 1 50000000 8  


  What the speedup results mean in terms of the number of cores on
  each lab machine, and how evenly your code divides the problem
  into equal-difficulty subproblems?
*/
