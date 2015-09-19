// count primes in a given interval in parallel

#include <pthread.h>
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// uint4 - unsigned 4-byte int
// Shorter to type than "unsigned int"
typedef unsigned int uint4;

pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

uint4 largestPrime = 0;

// a Job is a struct that represents the work that a thread
// needs to do.  In this case, it must describe what integers
// this thread needs to check.  It will also include the
// result - the number of primes that the thread found.
struct Job
{
  // TODO: Fill in this struct
  uint4 res;
  uint4 start;
  uint4 end;
  uint4 n;
  
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

  // TODO: fill in this function

  // If this thread finds a prime larger than largestPrime,
  // then set largestPrime.
  // However: largestPrime is a global variable, shared between the
  // threads.  We need to create a mutex that we can lock and unlock
  // as we modify largestPrime.
  // 
  // The need for a mutex here is slightly contrived; an equally good
  // solution would be for each thread to store the largest prime
  // it encounters in its Job struct, and the main thread could 
  // then select the largest of these.
  //
  // However, it's good practice for the marked questions to know how
  // to create and use a mutex, so we encourage you to use that solution
  // here.

  uint4 temp_max = 0;
  job->res = 0;
  for (uint4 i = job->start; i <= job->end; i++) {
    if (is_prime(i)) {
      job->res += 1;
      if (i > temp_max) {
        temp_max = i;
      }
    }
    
  }

  pthread_mutex_lock(&counter_mutex);
  if (temp_max > largestPrime) {
    largestPrime = temp_max;
  }
  pthread_mutex_unlock(&counter_mutex);

  return 0;
}

// compute number of primes in interval [a, b] using tn pthreads
uint4 num_primes(uint4 a, uint4 b, int tn)
{
  assert(a <= b);
  assert(tn > 0);
  pthread_t threads[tn];
  Job jobs[tn];

  // create work items and launch threads.
  //
  // There are many ways to divide the range [a,b] 
  // so that the threads handle non-overlapping
  // sets of integers to check.
  //
  // Our goal is to give each thread about the same
  // amount of work.  This lets us keep the CPUs busy,
  // and solve the problem as fast as possible.  If
  // some threads take much longer to run than others,
  // then we might leave some CPUs idle while the difficult
  // threads finish their work.
  //
  // In general, smaller numbers are easier to check than
  // larger ones, and even numbers are trivial to check.
  //
  // A good approach here is to pick an easy way to divide
  // the numbers across the jobs, and create a Job struct
  // on line 18 that can represent the numbers that a thread
  // should handle.  Your struct doesn't have to be complicated -
  // no arrays, queues, or other data structures!  3 or 4 ints is
  // enough.
  //
  // Once you've gotten your code working with a simple approach,
  // both with one thread and with many threads, see how long
  // it takes for a large range, like [1,5000000].  Then,
  // see if you can improve on how you divided the range
  // to make it run even faster.

  uint4 st = a;
  uint4 en = st + (b-a)/tn + 1;
  for (int i=0; i < tn; ++i) {
    // TODO: fill in jobs[i], then launch thread
    jobs[i].start = a;
    jobs[i].n = (b-a)/tn + 1;
    if (i == tn - 1) {
      if (en == b) {
        jobs[i].end = en;
      } else {
        jobs[i].end = en - 1;
      }
    } else {
      jobs[i].end = en;
    }
    st = en + 1;
    en = st + (b-a)/tn + 1;
  }


  // TODO: wait for threads to complete 
  for (int i = 0; i < tn; i++) {
    pthread_create(&threads[i], 0, prime_thread, &jobs[i]);
  }
  for (int i = 0; i < tn; i++) {
    pthread_join(threads[i], NULL);
  }

  // add up partial sums computed by the jobs
  uint4 primes = 0;
  for (int i=0; i < tn; ++i) {
    // TODO
    primes += jobs[i].res;
    
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
