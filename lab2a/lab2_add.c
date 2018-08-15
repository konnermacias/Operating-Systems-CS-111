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

struct timespec tp1, tp2;
int opt_yield = 0;
int numThreads = 1;
int numIters = 1;

/* Locks */
pthread_mutex_t addLock = PTHREAD_MUTEX_INITIALIZER;
typedef enum locks {NONE, MUTEX, SPIN, CandS} LOCK;
LOCK myLock = NONE;
int spinLock = 0;

void add(long long *pointer, long long value) {
    long long sum = *pointer + value;
    if (opt_yield){
        sched_yield();
    }
    *pointer = sum;
}

void mutex_add(long long *pointer, long long value) {
	pthread_mutex_lock(&addLock);
	add(pointer, value);
	pthread_mutex_unlock(&addLock);
}

void spin_add(long long *pointer, long long value) {
	while(__sync_lock_test_and_set(&spinLock,1) == 1);
	add(pointer, value);
	__sync_lock_release(&spinLock);
}

void compNswapAdd(long long *pointer, long long value) {
	long long com, sum;
	do{
		com = *pointer;
		sum = com + value;
		if (opt_yield){
			sched_yield();
		}
	}while(com != __sync_val_compare_and_swap(pointer, com, sum));
}

void errorMe(char* str){
	fprintf(stderr, "Error %s: %s\n", str, strerror(errno));
	exit(1);
}

struct thread_data{
	int numIts;
	long long *pointer;
};

void *tFunc(void *args){
	struct thread_data *myArgs = (struct thread_data *) args;
	int nIters = myArgs->numIts;
	long long *ptr = (long long *) myArgs->pointer;
	int i,j;

	/* add 1 to counter for spec num of times */
	for (i = 0; i < nIters; i++) {
		if (myLock == NONE){
			add(ptr, 1);
		}else if (myLock == MUTEX) {
			mutex_add(ptr, 1);
		}else if (myLock == SPIN) {
			spin_add(ptr, 1);
		}else if (myLock == CandS){
			compNswapAdd(ptr, 1);
		}
		
	}
	/* add -1 to counter for spec num of times */
	for (j = 0; j < nIters; j++){
		if (myLock == NONE){
			add(ptr, -1);
		}else if (myLock == MUTEX) {
			mutex_add(ptr, -1);
		}else if (myLock == SPIN) {
			spin_add(ptr, -1);
		}else if (myLock == CandS) {
			compNswapAdd(ptr, -1);
		}
	}

	pthread_exit(NULL);
}

/* print stats  */
/* format: name, #threads, #iters, totalOps, totRunTime,
			avgTime/op, totEndRun */
void printOut(int count){
	char* name;

	if (opt_yield) {
		switch(myLock) {
			case NONE:
				name = "add-yield-none";
				break;
			case MUTEX:
				name = "add-yield-m";
				break;
			case SPIN:
				name = "add-yield-s";
				break;
			case CandS:
				name = "add-yield-c";
				break;
		}
	}else{
		switch(myLock) {
			case NONE:
				name = "add-none";
				break;
			case MUTEX:
				name = "add-m";
				break;
			case SPIN:
				name = "add-s";
				break;
			case CandS:
				name = "add-c";
				break;
		}
	}

	int totalOps = numThreads*numIters*2;
  	long runTime = 1000000000*((long)tp2.tv_sec - (long)tp1.tv_sec) + tp2.tv_nsec - tp1.tv_nsec;
  	long avgTOp = runTime/totalOps;

  	fprintf(stdout, "%s,%d,%d,%d,%ld,%ld,%d\n",name,numThreads,numIters,totalOps,runTime,avgTOp,count);
}

int main(int argc, char **argv) {
  int c, rc, i;
  int option_index = 0;
  long long count = 0;


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
        	opt_yield = 1;
        	break;
        case 's':
        	switch(*optarg) {
        		case 'm':
        			myLock = MUTEX;
        			break;
        		case 's':
        			myLock = SPIN;
        			break;
        		case 'c':
        			myLock = CandS;
        			break;
        		default:
        			fprintf(stderr, "Invalid option. Correct Usage: lab2_add [--threads=num_threads] [--iterations=num_iterations] [--yield] [--sync=msc]\n");
        			exit(1);	
        	}
        	break;
      	default:
        	fprintf(stderr, "Invalid option. Correct Usage: lab2_add [--threads=num_threads] [--iterations=num_iterations] [--yield] [--sync=msc]\n");
        	exit(1);
    }
  }


  struct thread_data args; // declare struct
  args.numIts = numIters;
  args.pointer = &count;

  pthread_t threads[numThreads];

  clockid_t clk_id = CLOCK_MONOTONIC;
  clock_gettime(clk_id, &tp1); // note the starting time
  for (i = 0; i < numThreads; i++){
  	rc = pthread_create(&threads[i], NULL, tFunc, (void *) &args);
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

  clock_gettime(clk_id, &tp2); // get end time
  printOut(count); 
  exit(0);
}
