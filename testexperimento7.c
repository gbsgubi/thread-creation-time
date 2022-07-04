/***********************************************************************************
*
* This program was inspired by the solution developed by Florian Weizenegger
* and adapted so that it was possible to check the creation time of a thread
* by Gustavo Barbosa Silva
* Proposted by Carlos Miguel Tobar Toledo
*							Original Date: 25/08/2003 Date of Change: 14/06/2022
* To use this program you will have to compile it and change the value of NUM_THREADS
* with the value you want.
*       Purpose: The purpose of this program is to allow the student to understand
*       what is a thread, so as to distinguish it from value of NUM_THREADS
*       a process. In addition, the main commands are used to create
*		and thread manipulation.
*************************************************************************************/

/*
 * Includes Necessary 
 */

#include <pthread.h>		/* to handle threads */
#include <stdio.h>			
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

/*
 * Necessary Constants 
 */
#define NUM_THREADS     5
#define SIZEOFBUFFER    50
#define NO_OF_ITERATIONS 100
#define MICRO_PER_SECOND 1000000

/*
 * The pthread_t type allows the declaration of a variable that receives
 * an id when the thread is created. Later, this id can be
 * used in control commands for threads.
 * Two vectors for ids follow, for a number of threads equal to
 * NUM_THREADS constant
 */
pthread_t consumers[NUM_THREADS];
pthread_t producers[NUM_THREADS];

/*
 * Necessary Variables 
 */
int buffer[SIZEOFBUFFER];		/* this vector is used as a “circular” buffer */
int *start;				/* will point to the first buffer position */
int *rp;				/* is the pointer to the next item in the buffer to be consumed */
int *wp;				/* is the pointer to the next item in the buffer to be produced */
int cont_p = 0;         /* is a counter to control the number of items produced */
int cont_c = 0;         /* is a counter to control the number of items consumed */


/*
 * Routine to produce a toAdd item in the buffer 
 */
int myadd(int toAdd) {
  wp++;
 /* check if wp reached the last buffer position */
  if (wp == (start + SIZEOFBUFFER)) 
  wp = start; /* performs circularity in the buffer */
  /* check if the buffer is not full */
  if ( *wp == 0) {
  *wp = toAdd;
  return 1;
  } else return 0;
}

/*
 * Routine to consume an item from the buffer and put it in retValue 
 */
 int myremove() {
   int retValue;
   /* check that the buffer is not empty */
   if (*rp != 0) {
       retValue = *rp;
       *rp = 0;
       rp++;
       /* check if rp reached the last buffer position */
       if (rp == (start + SIZEOFBUFFER)) 
           rp = start;      /* performs circularity in the buffer */
       return retValue;
    } else return 0;
  }

/*
 * The produce routine is responsible for calling myadd so that it is 
 * put the value 10 in a buffer position NO_OF_ITERATIONS times
 */
void *produce(void *threadid)
{
  int ret;
  int sum = 0;

  printf("Producer #%d started...\n", threadid);

  while (cont_p < NO_OF_ITERATIONS) {
    ret = myadd(10);
    if (ret) {
    /* 
     * Question 1: why is ret not being compared to some value?
     */
      cont_p++;
      sum += 10;
    }
  }
  printf("Sum produced by the Producer #%d : %d\n", threadid, sum);
  pthread_exit(NULL);

}

/*
 * The routine consume is responsible for calling myremove so that it is
 * returning one of the existing values ​​in the NO_OF_ITERATIONS buffer times 
 */
void *consume(void *threadid)
{
  int ret;
  int sum = 0;

  printf("Consumer #%d started...\n", threadid);

  while (cont_c < NO_OF_ITERATIONS) {
    ret = myremove();
    if (ret != 0) {
      cont_c++;
      sum += ret;
    }
  }
  printf("Sum of what was consumed by the Consumer #%d : %d\n", threadid, sum);
  pthread_exit(NULL);
}

/*
 * Main Routine (which is also the main thread when executed) 
 */
int main(int argc, char *argv[])
{
  int tp, tc;
  int i;
  float drift;
  struct timeval start_time;
  struct timeval stop_time;
  
  gettimeofday( &start_time, NULL);
  
  start = &buffer[0];
  wp = start + SIZEOFBUFFER - 1;
  rp = start;

  for (i=0;i<SIZEOFBUFFER;i++) {
      *rp = 0;
      rp++;
  };
  
  rp = start;
  
  for (i=0;i<NUM_THREADS;i++) {
    /* try to create a consumer thread */
    tc = pthread_create(&consumers[i], NULL, consume, (void *)i+1);

    /* 
     * Question 2: what each of the arguments used with pthread_create is for?
     */

    if (tc) {
      printf("ERROR: Unable to create a consumer thread\n");
      exit(-1);
    }
    /* try to create a producer thread*/
    tp = pthread_create(&producers[i], NULL, produce, (void *)i+1);
    if (tp) {
      printf("ERROR: Unable to create a producer thread\n");
      exit(-1);
    }
  }
  
  gettimeofday( &stop_time, NULL);
  drift = (float)(stop_time.tv_sec - start_time.tv_sec);
  drift += (stop_time.tv_usec - start_time.tv_usec)/(float)MICRO_PER_SECOND;
  printf("\nTotal time: %.9f sec\n\n", drift);
  printf("Terminating the main() thread\n");
  pthread_exit(NULL);

/* 
 * Question 3: What happens to the created threads, if they still
 * are being executed and the thread that created them terminates
 * via a pthread_exit()?
 */

/*
 * Question 4: Same as the previous question, if the end occurs through
 * from an exit()?
 */
}
