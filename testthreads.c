
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/wait.h>
#include <pthread.h>

#include "daf_threads.h"

typedef struct threadparm {
   int duration;
   thread_workitem_t *pthread_workitem;
} threadparm_t;

void helloworld(void *p) {

  thread_workitem_t *pthread_workitem;

  pthread_workitem = ((threadparm_t *) p)->pthread_workitem;
   
  printf("hello world - thread %d, sleeping for %d\n", pthread_workitem->threadnum, ((threadparm_t *) p)->duration);
  
  add_item_to_trace_table( pthread_workitem, LOCK_MUTEX, LOCK, 1234, 0, 0, 0);

 
  sleep(((threadparm_t *) p)->duration);
 
  exit_thread( pthread_workitem, 33 + pthread_workitem->threadnum);

}


int main(int argc, char **argv) {

  #define NUM_THREADS 2
  thread_workitem_t thread_workitems[NUM_THREADS];
  int i;
  int num_unfinished_threads;

  threadparm_t threadparm[NUM_THREADS];

  if (init_thread_environment() != 0) {
    printf("problem running init_thread_environment()\n");
    exit(1);
  }

  for (i=0; i< NUM_THREADS; i++) {

     initialise_thread_workitem(&(thread_workitems[i]), i, 1024, FALSE);
     threadparm[i].pthread_workitem = &(thread_workitems[i]);
     threadparm[i].duration = i;

     create_thread((void *(*)(void *)) &helloworld, 
                   (void *) &(threadparm[i]),
                    &(thread_workitems[i]));

  }
  
  sleep(1);

  do {

     num_unfinished_threads = 0;
     for (i=0; i< NUM_THREADS; i++) {
  
        if (thread_workitems[i].active) {
           if (thread_workitems[i].terminated) {
              printf("Thread %d  threadnum %d returned %d\n", i, 
                     thread_workitems[i].threadnum, thread_workitems[i].thread_exit_code.value);
              thread_workitems[i].active = FALSE;
           } else {
              num_unfinished_threads++;
           }
        }
     }
     printf("There are %d unfinished threads\n", num_unfinished_threads);

     sleep(1);
  } while (num_unfinished_threads != 0);

  for (i=0; i< NUM_THREADS; i++) {

     if (i == 0) print_trace_buffer(&(thread_workitems[i]));

     deinitialise_thread_workitem(&(thread_workitems[i]));
  }

  exit(0);

}

