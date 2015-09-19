// this code should be compiled with
// g++ -Wall -Wextra -Wconversion -O3 insertion.cpp -o insertion -lpthread

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <list>
#include <string>

using namespace std;

// global variables

list<string> wordList;
const int numWords = 5;  // each array contains 5 words

// TODO: declare a mutex here


void * insert(void *);

int main()
{

  const int TN = 4;   // number of arrays and threads to create
  
  // Create 4 arrays, each array contains 5 words
  string words[TN][numWords] =
    { {"promotional", "drafted", "slog", "cronkite", "camber"},
      {"scatted", "overused", "promiscuity", "legatee", "bettor"},
      {"reentering", "satirist", "sawmill", "upchucks", "unseasonable"},
      {"testimonials", "upstairs", "discrepancies", "mascaraed", "darcy"} };
      
  
  // We use each thread to add an array of words into the list wordList

  pthread_t threads[TN];
  
  // create TN threads
  for (int i=0; i < TN; i++) {
    pthread_create(&threads[i], 0, insert, (void *) &words[i]);
  }
  
  // wait until all threads are complete before main() continues
  for (int i=0; i < TN; i++) {
    pthread_join(threads[i], 0);
    printf("Thread completed.\n");
  }

  // sort the linked list
  wordList.sort();
  
  // Print the resulting list of words in wordList
  // The words should be in sorted order
  printf("The list of words:\n");

  list<string>::iterator itList = wordList.begin();
  list<string>::iterator endList = wordList.end();
  for (; itList != endList; itList++)
  {
    printf("Word: %s\n", (*itList).c_str());
  }
  
  
  return 0;
}


// This function runs in a thread
// Adds words from an array into the linked list wordList
void * insert(void * param)
{

  printf("Thread created.\n");
  
  string *words = (string *) param;   // an array of words

  // TODO: for each word in words[], add it to the end of wordList
  // You will need to use a mutex to ensure the add process does not have collisions.
  for (int i = 0; i<numWords; i++)
  {


  }
  
  return 0;
}
