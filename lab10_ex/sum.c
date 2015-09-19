// this code should be compiled with
// g++ -Wall -Wextra -Wconversion -O3 sum.c -o sum -lpthread

#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// a Job is a struct that represents the work that a thread
// needs to do.  In this case, struct Job should contain
// a pointer to the array, the start and the end indices, 
// and a variable storing the result
struct Job
{
  // TODO: Fill in this struct
  
};


// All threads will run this function when started.
// The thread retrieves the job that was passed in as an argument, 
// which contains the array, the start and the end indices of array,
// and a variable storing the result.
// You should calculate the sum of array elements between start 
// and end indices, then save the answer back into the result
// variable in the job struct.
static void *sum_thread(void *data)
{
  Job *job = (Job *) data;
  // TODO: Fill in this function
  
  
  
  return 0;
}


// This function sums up n elements in array A using TN threads
// returns the sum as a double
double sum(double A[], int n, int TN)
{
  assert(n > 0);
  assert(TN > 0);
  
  pthread_t threads[TN];  // create TN number of threads
  Job jobs[TN];           // create TN number of jobs, each job is passed into a thread
  
  for (int i=0; i < TN; ++i) {
  
    // TODO: add code here
    // Here you have array A, containing n elements.
    // You need to divide the n elements among TN jobs.
    // Each job calculates the sum of array elements from a start to
    // an end index. For each job jobs[i], you need to set the
    // approprite values for the variables in the Job struct, such as
    // the start and end indices.
    

    
	
	
    // launch thread with parameter jobs[i]
    pthread_create(&threads[i], 0, sum_thread, &jobs[i]);
  }

  // Wait for threads to complete (join),
  // After all threads complete, add up partial sums to sum
  double sum = 0;
  for (int i=0; i < TN; ++i) {
    // TODO: add code here
	
	
	
  }

  return sum;
}



int main(int argc, char *argv[])
{
  if (argc != 3) {
    printf("usage: %s array-length threads\n", argv[0]);
    exit(1);
  }

  int length = 100000;
  int TN = 1;

  length = atoi(argv[1]);
  TN = atoi(argv[2]);

  assert(length >= 1);
  assert(TN > 0);
  
  // create and initialize array of doubles
  double *A = new double[length];
  int count = 0;
  for (int i=0; i < length; ++i) {
    A[i] = count++;
  }

  // calculate sum of array using sum() function
  double result;
  for (int i=0; i < 1000; ++i) {
    result = sum(A, length, TN);
  }
  
  printf("Result = %f\n", result);
  delete [] A;
  
  return 0;
}

/*
  Copy and paste the output of the test results here:

 


  
  How many threads give you the fastest result?
  
  Answer:
  
*/
