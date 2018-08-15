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
#include "mraa.h"
#include <string.h>
#include <mraa/gpio.h>

#define BUF_SIZE 256

/* global variables */
sig_atomic_t volatile flag = 1;
const int B = 4275;               // B value of the thermistor
const int R0 = 100000;            // R0 = 100k
int logFd = -1;

/* error function */
void errorMe(char* str){
        fprintf(stderr, "Error %s: %s\n", str, strerror(errno));
        exit(1);
}

/* handling ^C */
void do_when_interrupted(int sig) {
	if (sig == SIGINT) {
		flag = 0;
	}
}

/* handling a button click */
void do_when_button_clicked(){
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
	int secInterval = 1;
	int i = 0;
	int logFlag = 0;
	int on = 1;
	int index = 0;
	uint16_t value;	
	int c;
	char *log_file;

	typedef enum scale {F, C} SCALE;
	SCALE *temp;
	SCALE x = F;
	temp = &x;

	static struct option long_options[] = {
		{"scale", optional_argument, 0,'s'},
    		{"period", optional_argument,0,'p'},
    		{"log", optional_argument,0,'l'},
    		{0,0,0,0}
	};

	while ((c = getopt_long(argc, argv, "spl", long_options, &option_index)) != -1) {
    	switch(c) {
      		case 's':
        		if (*optarg == 'C') {
       	  			*temp = C;
    			} else if (*optarg != 'F') {
    	  			fprintf(stderr, "Invalid option. Correct Usage: lab4b [--scale=CF] [--period=num_secs] [--log=filename]\n");
    	  			exit(1);
    			}
    	  		break;
    		case 'p':
    			secInterval = (int)atoi(optarg);
    			break;
      		case 'l':
    			log_file = optarg;
    			logFlag = 1;
    			break;
      		default:
    			fprintf(stderr, "Invalid option. Correct Usage: lab4b [--scale=CF] [--period=num_secs] [--log=filename]\n");
    			exit(1);
    		}
  	}

  	if (logFlag) {
  		if ((logFd = creat(log_file, S_IRWXU)) < 0) {
  			errorMe("opening or creating logfile");
  		}
  	}

	/* temperature sensor */
	mraa_aio_context aio = mraa_aio_init(1); // aio port 1
	if (aio == NULL) {
		fprintf(stderr, "Failed to initialize AIO\n");
		mraa_deinit();
		exit(1);
	}

	/* button */	
	mraa_gpio_context button = mraa_gpio_init(60);
        if (button == NULL) {
               	fprintf(stderr, "Failed to initialize AIO\n");
                mraa_deinit();
                exit(1);
        }
	
	/* handle when button is pushed */
	mraa_gpio_isr(button, MRAA_GPIO_EDGE_RISING, &do_when_button_clicked, NULL);

	/* handle when ^C is received */
	signal(SIGINT, do_when_interrupted);

	/* polling */	
	struct pollfd pfd[1];
	pfd[0].fd = STDIN_FILENO;	
	pfd[0].events = POLLIN | POLLHUP | POLLERR;
	int pollRes, readRes;

	/* initialize buffers */
	char stdinBuf[BUF_SIZE];
	char myInput[BUF_SIZE];
	memset(stdinBuf, 0, BUF_SIZE);
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
	
			printf("%s %.1f\n",tm,temperature);
                	if (logFlag) {
				dprintf(logFd,"%s %.1f\n",tm,temperature);
			}
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
	                if ((readRes = read(STDIN_FILENO, stdinBuf, BUF_SIZE)) == -1 ) {
	                	errorMe("reading");
	                } else if (readRes == 0) {
				fprintf(stderr, "Received EOF\n");
				mraa_aio_close(aio);
        			mraa_gpio_close(button);
			}

			for (i = 0; i < readRes && index < BUF_SIZE; i++) {
				if (stdinBuf[i] == '\n') {
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

					if (logFlag) {
						dprintf(logFd,"%s\n",myInput);
					} 	
				
					index = 0;
					memset(myInput, 0, BUF_SIZE);
				} else {
					myInput[index] = stdinBuf[i];
					index++;
				}
			}
                }

	}
	
	/* handle shutdown */
	time_t clk = time(NULL);
        char *tm = editTimestamp(ctime(&clk));
	printf("%s SHUTDOWN\n",tm);
	if (logFlag) {
		dprintf(logFd, "%s SHUTDOWN\n",tm);
	}
	free(tm);
	/* close aio */
	mraa_aio_close(aio);
	mraa_gpio_close(button);
	exit(0);
}
