// NAME: Konner Macias
// EMAIL: konnermacias@g.ucla.edu
// ID: 004603916

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <poll.h>
#include <mraa/aio.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include "mraa.h"
#include <string.h>
#include <mraa/gpio.h>
#include <netdb.h>
#include <netinet/in.h>

#define BUF_SIZE 256

/* FOR TCP */
char *id = NULL;
char *host = NULL;
int sockFd = -1;
int portnum = 0;

/* global variables */
sig_atomic_t volatile flag = 1;
const int B = 4275;               // B value of the thermistor
const int R0 = 100000;            // R0 = 100k
int logFd = -1;

/* error function */
void errorMe(char* str){
        fprintf(stderr, "Error %s: %s\n", str, strerror(errno));
        exit(2);
}

/* handling ^C */
void do_when_interrupted(int sig) {
	if (sig == SIGINT)
		flag = 0;
}

/* converting temp to F */
float C_to_F(float cel) {
	return cel*(9.0/5.0) + 32;
}

/* making the time stamp correct */
char* editTimestamp(char * tms) {
	return strndup(tms + 11,8);
}

int main(int argc, char** argv) {
	int option_index = 0;
	int i = 0;
	int on = 1;
	int index = 0;
	uint16_t value;	
	int c;
	char *log_file;
	int secInterval = 1;

	typedef enum scale {F, C} SCALE;
	SCALE *temp;
	SCALE x = F;
	temp = &x;

	/* Scan through arguments to get port number */
	for (i = 1; i < argc; i++) {
		if (argv[i][0] != '-') {
			if ((portnum = strtol(argv[i],NULL,10)) < 0) {
				fprintf(stderr, "Error! Invalid Port Number: %s\n", strerror(errno));
        			exit(1);
			}
			break;
		}
	}

	static struct option long_options[] = {
		{"id",required_argument,0,'i'},
    		{"host",required_argument,0,'h'},
    		{"log",required_argument,0,'l'},
		{"period",optional_argument,0,'p'},
		{"scale",optional_argument,0,'s'},
    		{0,0,0,0}
	};

	while ((c = getopt_long(argc, argv, "ihlps", long_options, &option_index)) != -1) {
    	switch(c) {
      		case 'i':
        		if (strlen(optarg) != 9) {
				fprintf(stderr, "Error! ID length incorrect: %s\n", strerror(errno));
        			exit(1);
        		}
        		id = optarg;
        		break;
    		case 'h':
    			host = optarg;
    			break;
      		case 'l':
    			log_file = optarg;
    			break;
      		case 'p':
			secInterval = (int)atoi(optarg);
			break;
		case 's':
			if (*optarg == 'C') {
				*temp = C;
			} else if (*optarg != 'F') {
				fprintf(stderr, "Error! Invalid option. Scale can only be C or F: %s\n",strerror(errno));
			        exit(1);
			}
			break;
		default:
    			fprintf(stderr, "Invalid option. Correct Usage: ./lab4c_tcp portnumber --id=ID-number --host=name --log=filename [--scale=CF] [--period=num_seconds]\n");
    			exit(1);
    		}
  	}

  	if ((logFd = creat(log_file, S_IRWXU)) < 0) {
  		errorMe("opening or creating logfile");
  	}


	/* set socket */
  	if ((sockFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
  		errorMe("using socket()");
  	}

	/* set up server */
        struct hostent* server;

  	if ((server = gethostbyname(host)) == NULL) {
  		errorMe("using gethostbyname()");
  	}

	/* set up connection */
	struct sockaddr_in addr;
  	memset(&addr, 0, sizeof(addr));
  	addr.sin_family = AF_INET;
  	memcpy((char*) &addr.sin_addr, (char *)server->h_addr, server->h_length);
  	addr.sin_port = htons(portnum);

  	if (connect(sockFd, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
  		errorMe("using connect()");
  	}
	
	/* set up temperature sensor */
	mraa_aio_context aio = mraa_aio_init(0); // aio port 0
	if (aio == NULL) {
		fprintf(stderr, "Failed to initialize AIO\n");
		mraa_deinit();
		exit(1);
	}
	/* handle when ^C is received */
	signal(SIGINT, do_when_interrupted);


	/* immediately send (and log) ID */
        dprintf(sockFd, "ID=%s\n",id);
        dprintf(logFd, "ID=%s\n",id);


	/* polling */	
	struct pollfd pfd[1];
	pfd[0].fd = sockFd;	
	pfd[0].events = POLLIN | POLLHUP | POLLERR;
	int pollRes, readRes;

	/* initialize buffers */
	char sockBuf[BUF_SIZE];
	char myInput[BUF_SIZE];
	memset(sockBuf, 0, BUF_SIZE);
	memset(myInput, 0, BUF_SIZE);

	while(flag) {
		if (on) {
			/* wait for spec num secs */
			sleep(secInterval);
			
			/* get current time */
			time_t clk = time(NULL);
			char *tm = editTimestamp(ctime(&clk));		
			/* temperature conversion from sensor */
			value = mraa_aio_read(aio);
			float R = 1023.0/value - 1.0;
        		R = R0*R;
        		float temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15;
			if (*temp != C) {
				temperature = C_to_F(temperature);
       			}
	
			dprintf(sockFd,"%s %.1f\n",tm,temperature);
			dprintf(logFd,"%s %.1f\n",tm,temperature);
			free(tm);
		}
		
		pollRes = poll(pfd,1,0);		
		if (pollRes == 0) continue;
	    	if (pollRes < 0) {
	       		errorMe("polling");
	    	}
	    	
		/* received input via terminal */
	    	if ((pfd[0].revents&POLLIN) == POLLIN) {
	        	/* get input */
	        	if ((readRes = read(sockFd, sockBuf, BUF_SIZE)) == -1 ) {
	          		errorMe("reading");
	        	} else if (readRes == 0) {
				fprintf(stderr, "Received EOF\n");
				mraa_aio_close(aio);
			}

			for (i = 0; i < readRes && index < BUF_SIZE; i++) {
				if (sockBuf[i] == '\n') {
					/* Analyze our input */
					if (strncmp(myInput,"SCALE=",sizeof(char)*6 ) == 0) {
						if (myInput[6] == 'C') {
							*temp = C;
						}else if (myInput[6] == 'F') {
							*temp = F;
						}
					} else if (strncmp(myInput,"PERIOD=",sizeof(char)*7) == 0) {
						const char* sec = &myInput[7];
						secInterval = atoi(sec);
					} else if (strcmp(myInput,"STOP") == 0) {
						on = 0;
					} else if (strcmp(myInput,"START") == 0) {
						on = 1;
					} else if (strcmp(myInput,"OFF") == 0) {
						flag = 0;
					}

					dprintf(logFd,"%s\n",myInput);
					index = 0;
					memset(myInput, 0, BUF_SIZE);
				} else {
					myInput[index] = sockBuf[i];
					index++;
				}
			}
		}
	}

	/* handle shutdown */
	time_t clk = time(NULL);
    	char *tm = editTimestamp(ctime(&clk));
	dprintf(sockFd,"%s SHUTDOWN\n",tm);
	dprintf(logFd, "%s SHUTDOWN\n",tm);
	free(tm);
	
	/* close aio and socket */
	mraa_aio_close(aio);
	exit(0);
}
