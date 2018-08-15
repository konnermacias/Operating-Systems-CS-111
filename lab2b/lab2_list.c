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

/* globals */
struct timespec tp1, tp2;
int opt_yield;
int numThreads = 1;
int numIters = 1;
int numLists = 1;

/* define a struct of our sublists */
typedef struct sub_list{
        SortedList_t* list;
        pthread_mutex_t addLock;
        int spinLock;
} sub_List;

/* list elemenets */
sub_List *sub_lists;
SortedListElement_t **elms;
int num_elms;

/* locks */
typedef enum locks {NONE, MUTEX, SPIN} LOCK;
LOCK myLock = NONE;
int spinLock = 0;
unsigned long *lockTimes;

/* lock up lists, and get time */
void lockItUp(int index, int tid){
	if (myLock == MUTEX) {
		struct timespec start, end;
        	clock_gettime(CLOCK_MONOTONIC, &start);
                pthread_mutex_lock(&sub_lists[index].addLock);
                clock_gettime(CLOCK_MONOTONIC, &end);
		long lockTime = 1000000000*((long)end.tv_sec - (long)start.tv_sec) + end.tv_nsec - start.tv_nsec;
		lockTimes[tid] += (unsigned long) lockTime;
        }
        if (myLock == SPIN) {
		struct timespec start, end;
                clock_gettime(CLOCK_MONOTONIC, &start);
                while(__sync_lock_test_and_set(&sub_lists[index].spinLock,1));
                clock_gettime(CLOCK_MONOTONIC, &end);
		long lockTime = 1000000000*((long)end.tv_sec - (long)start.tv_sec) + end.tv_nsec - start.tv_nsec;
        	lockTimes[tid] += (unsigned long)lockTime;
	}
}

/* unlock the lists */
void unlockItUp(int index) {
	if (myLock ==  MUTEX){
                pthread_mutex_unlock(&sub_lists[index].addLock);
        }
        if (myLock == SPIN){
                __sync_lock_release(&sub_lists[index].spinLock);
        }
}
		
/* taken from stackOverflow, credit is in README */
unsigned int
hash(const char *str)
{
    unsigned int hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}


/* to be called with pthread_create */
void * tFunc(void *args){
	int tid = *(int *) args;
	SortedListElement_t *temp;
	int i, rc, k;
	int listLength = 0;

	for (i = tid; i < num_elms; i += numThreads){
		k = hash(elms[i]->key) % numLists;
		lockItUp(k, tid);
		SortedList_insert(sub_lists[k].list, elms[i]);
		unlockItUp(k);		
	}

	/* check for corrupted lists */
	for (i = 0; i < numLists; i++){
		lockItUp(i, tid);
		rc = SortedList_length(sub_lists[i].list);
		unlockItUp(i);
		if (rc == -1) {
			fprintf(stderr, "Error! Corrupted list length\n");
			exit(2);
		}
		listLength += rc; // sum lengths across sub-lists!
	}

	for (i = tid; i < num_elms; i += numThreads){
		k = hash(elms[i]->key) % numLists;
		lockItUp(k, tid);
		temp = SortedList_lookup(sub_lists[k].list, elms[i]->key);
		if (temp == NULL){
			fprintf(stderr, "Error using SortedList_lookup\n");
			exit(2);
		}

		rc = SortedList_delete(temp);
		if (rc){
			fprintf(stderr, "Error using SortedList_delete\n");
			exit(2);
		}
		unlockItUp(k);
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
	unsigned long totLockTime = 0;
	if (myLock != NONE) {
		for (int i = 0; i < numThreads; i++) {
			totLockTime += lockTimes[i];
		}
	}

	int totalOps = numThreads*numIters*3;
  	long runTime = 1000000000*((long)tp2.tv_sec - (long)tp1.tv_sec) + tp2.tv_nsec - tp1.tv_nsec;
  	long avgTOp = runTime/totalOps;
	long avWaitpLock = totLockTime / totalOps;

  	fprintf(stdout,",%d,%d,%d,%d,%ld,%ld,%ld\n",numThreads,numIters,numLists,totalOps,runTime,avgTOp,avWaitpLock);
}

int main(int argc, char **argv) {
	int c, rc, i, j, len, charLen;
	int option_index = 0;

  static struct option long_options[] = {
	{"threads", optional_argument, 0,'t'},
    {"iterations", optional_argument,0,'i'},
    {"yield", optional_argument,0,'y'},
    {"sync", optional_argument,0,'s'},
    {"lists",optional_argument,0,'l'},
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
        				fprintf(stderr, "Invalid option. Correct Usage: lab2_list [--threads=num_threads] [--iterations=num_iterations] [--lists=num_lists] [--yield=idl] [--sync=ms]\n");
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
        			fprintf(stderr, "Invalid option. Correct Usage: lab2_list [--threads=num_threads] [--iterations=num_iterations] [--lists=num_lists] [--yield=idl] [--sync=ms]\n");
        			exit(1);	
        	}
        	break;
	case 'l':
		numLists = (int)atoi(optarg);
		break;
      	default:
        	fprintf(stderr, "Invalid option. Correct Usage: lab2_list [--threads=num_threads] [--iterations=num_iterations] [--lists=num_lists] [--yield=idl] [--sync=ms]\n");
        	exit(1);
    }
  }

  /* catch seg faults */
  signal(SIGSEGV, sighandler);

  /* define number of elements */
  num_elms = numThreads * numIters;
  elms = malloc(num_elms * sizeof(SortedListElement_t*));
  lockTimes = malloc(numThreads * sizeof(long));

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

  /* break into sublists with own list header */
  sub_lists = malloc(numLists * sizeof(sub_List));
  for (i = 0; i < numLists; i++) {
	sub_lists[i].list = malloc(sizeof(SortedList_t));
	sub_lists[i].list->next = sub_lists[i].list;
	sub_lists[i].list->prev = sub_lists[i].list;
	sub_lists[i].list->key = NULL;
	sub_lists[i].spinLock = 0;
	pthread_mutex_init(&sub_lists[i].addLock, NULL);
  } 

  /* pthreads */
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

  /* confirm each list is empty */
  for (i = 0; i < numLists; i++) {
	if (SortedList_length(sub_lists[i].list) != 0){
  		fprintf(stderr,"Error! Length of list is not zero");
  		exit(2);
  	}
  }

  printOut(); 
  exit(0);
}
