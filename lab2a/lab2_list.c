// NAME: Konner Macias
// EMAIL: konnermacias@g.ucla.edu
// ID: 004603916

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include <signal.h>
#include "SortedList.h"

struct timespec tp1, tp2;
int opt_yield;

int numThreads = 1;
int numIters = 1;
int numLists = 1;

/* list elemenets */
SortedList_t *list;
SortedListElement_t **elms;
int num_elms;

/* locks */
pthread_mutex_t addLock = PTHREAD_MUTEX_INITIALIZER;
typedef enum locks {NONE, MUTEX, SPIN} LOCK;
LOCK myLock = NONE;
int spinLock = 0;

/* to be called with pthread_create */
void *tFunc(void *args){
	int tid = *(int *) args;
	SortedListElement_t *temp;
	int i, rc;

	/* Lock it up! */
	if (myLock ==  MUTEX){
		pthread_mutex_lock(&addLock);
	}
	if (myLock == SPIN){
		while(__sync_lock_test_and_set(&spinLock,1));
	}


	for (i = tid; i < num_elms; i += numThreads){
		SortedList_insert(list, elms[i]);
	}

	if (SortedList_length(list) == -1){
		fprintf(stderr, "Error! Corrupted list length\n");
		exit(2);
	}
	
	for (i = tid; i < num_elms; i += numThreads){
		temp = SortedList_lookup(list, elms[i]->key);
		if (temp == NULL){
			fprintf(stderr, "Error using SortedList_lookup\n");
			exit(2);
		}

		rc = SortedList_delete(temp);
		if (rc){
			fprintf(stderr, "Error using SortedList_delete\n");
			exit(2);
		}
	}

	/* unlock that */
	if (myLock ==  MUTEX){
		pthread_mutex_unlock(&addLock);
	}
	if (myLock == SPIN){
		__sync_lock_release(&spinLock);
	}

	pthread_exit(NULL);
}


void sighandler(int signum) {
	if (signum == SIGSEGV){
		fprintf(stderr, "Segmentation fault: %s\n", strerror(signum));
		exit(2);
	}
}

void errorMe(char* str){
	fprintf(stderr, "Error %s: %s\n", str, strerror(errno));
	exit(1);
}

/* print stats  
 format: name, #threads, #iters, totalOps, totRunTime,
			avgTime/op */
void printOut(){
	fprintf(stdout,"list-");

	if (opt_yield == 0){
		fprintf(stdout,"none");
	}
	else{
		if (opt_yield & INSERT_YIELD){
			fprintf(stdout,"i");
		}
		if (opt_yield & DELETE_YIELD){
			fprintf(stdout,"d");
		}
		if (opt_yield & LOOKUP_YIELD){
			fprintf(stdout,"l");
		}
	}

	if (myLock == NONE){
		fprintf(stdout,"-none");
	}
	else if (myLock == MUTEX){
		fprintf(stdout,"-m");
	}
	else if (myLock == SPIN){
		fprintf(stdout,"-s");
	}

	int totalOps = numThreads*numIters*3;
  	long runTime = 1000000000*((long)tp2.tv_sec - (long)tp1.tv_sec) + tp2.tv_nsec - tp1.tv_nsec;
  	long avgTOp = runTime/totalOps;

  	fprintf(stdout,",%d,%d,%d,%d,%ld,%ld\n",numThreads,numIters,numLists,totalOps,runTime,avgTOp);
}

int main(int argc, char **argv) {
	int c, rc, i, j, len, charLen;
	int option_index = 0;

  static struct option long_options[] = {
	{"threads", optional_argument, 0,'t'},
    {"iterations", optional_argument,0,'i'},
    {"yield", optional_argument,0,'y'},
    {"sync", optional_argument,0,'s'},
    {0,0,0,0}
  };

  while ((c = getopt_long(argc, argv, "tiys", long_options, &option_index)) != -1) {
    switch(c) {
    	case 't':
        	numThreads = (int)atoi(optarg);
        	break;
      	case 'i':
        	numIters = (int)atoi(optarg);
        	break;
        case 'y':
        	len = strlen(optarg);
        	for (i = 0; i < len; i++){
        		switch(optarg[i]){
        			case 'i':
        				opt_yield |= INSERT_YIELD;
        				break;
        			case 'd':
        				opt_yield |= DELETE_YIELD;
        				break;
        			case 'l':
        				opt_yield |= LOOKUP_YIELD;
        				break;
        			default:
        				fprintf(stderr, "Invalid option. Correct Usage: lab2_list [--threads=num_threads] [--iterations=num_iterations] [--yield=idl] [--sync=ms]\n");
        				exit(1);
        		}
        	}
        	break;
        case 's':
        	switch(*optarg) {
        		case 'm':
        			myLock = MUTEX;
        			break;
        		case 's':
        			myLock = SPIN;
        			break;
        		default:
        			fprintf(stderr, "Invalid option. Correct Usage: lab2_list [--threads=num_threads] [--iterations=num_iterations] [--yield=idl] [--sync=ms]\n");
        			exit(1);	
        	}
        	break;
      	default:
        	fprintf(stderr, "Invalid option. Correct Usage: lab2_list [--threads=num_threads] [--iterations=num_iterations] [--yield=idl] [--sync=ms]\n");
        	exit(1);
    }
  }

  /* catch seg faults */
  signal(SIGSEGV, sighandler);

  /* initialize empty list */
  list = malloc(sizeof(SortedList_t));
  list->next = list;
  list->prev = list;
  list->key = NULL;

  num_elms = numThreads * numIters;
  elms = malloc(num_elms * sizeof(SortedListElement_t*));

  /* initialize to NULL to avoid segmentation fault */
  for (i = 0; i < num_elms; i++){
  	elms[i] = malloc(sizeof(SortedListElement_t));
  	elms[i]->next = NULL;
  	elms[i]->prev = NULL;
  }

  /* generate random elms */
  for (i = 0; i < num_elms; i++){
  	charLen = random() % 20;
  	char *key = malloc((charLen+1)*sizeof(char));
  	for (j = 0; j < charLen; j++){
  		key[j] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[random () % 26];
  	}
  	key[charLen] = '\0';
  	elms[i]->key = key;
  }

  pthread_t threads[numThreads];
  int *tids = malloc(numThreads * sizeof(int));


  /* note start time */
  clockid_t clk_id = CLOCK_MONOTONIC;
  clock_gettime(clk_id, &tp1);

  for (i = 0; i < numThreads; i++){
  	tids[i] = i;
  	rc = pthread_create(&threads[i], NULL, tFunc, (void *)(tids + i));
  	if (rc){
  		errorMe("using pthread_create()");
  	}
  }

  for (i = 0; i < numThreads; i++){
  	rc = pthread_join(threads[i], NULL);
  	if (rc) {
  		errorMe("using pthread_join()");
  	}
  }

  clock_gettime(clk_id, &tp2);

  if (SortedList_length(list) != 0){
  	fprintf(stderr,"Error! Length of list is not zero");
  	exit(2);
  }

  printOut(); 
  exit(0);
}
