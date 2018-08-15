// NAME: Konner Macias
// EMAIL: konnermacias@g.ucla.edu
// ID: 004603916

#include<stdio.h>
#include<getopt.h>
#include<fcntl.h>
#include<stdlib.h>
#include<termios.h>
#include<unistd.h>
#include<pthread.h>
#include<signal.h>
#include<poll.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>

struct termios initTerm, newTerm;
int toChild[2];
int fromChild[2];
int shellFlag = 0;
int debugFlag = 0;
pid_t pid;

// error message function that saved me a lot of time
void errorMe(char* str){
	fprintf(stderr, "Error %s: %s\n", str, strerror(errno));
	exit(1);
}

void sighandler(int signum){
	if (signum == SIGINT && shellFlag){
		// use kill(2) to send a SIGINT to the shell
		if (kill(pid, SIGINT) == -1){
			errorMe("killing process");
		}
	}

	if (signum == SIGPIPE){
		exit(0); // success
	}
}
			
void resetSettings(){
	// set terminal modes back to what they were originally
	if (tcsetattr(STDIN_FILENO, TCSANOW, &initTerm) == -1){
		errorMe("resetting terminal settings");
	}
	
	// if we are operating in shell
	if (shellFlag){
		int val;
		pid_t waitForIt;
		if ((waitForIt = waitpid(pid, &val, 0)) == -1){
			errorMe("with waitpid");
		}
		// collect shell's exit status
		fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WTERMSIG(val), WEXITSTATUS(val));
	}
		
}


void writeThat(int fd, char* buf, int numBytes){
	int i;
	char c;
	for (i = 0; i < numBytes; i++){
		c = buf[i];
		if (c == 0x04){ // ^D
			if (shellFlag){
				// close pipe to shell
				close(toChild[1]);				
 			}else{
				exit(0); // shutdown on ^D
			}
		}else if (c == 0x03){
			if (shellFlag){
				kill(pid, SIGINT); // use kill to send a SIGINT to shell process
			}
		}else if (c == 0x0D || c == 0x0A){
			if (fd == STDOUT_FILENO){
				char special[2];
				special[0] = '\r';
				special[1] = '\n';
				if (write(fd, special, sizeof(special)) == -1){
					errorMe("writing");
				}
			}else{
				// go to shell as <lf>
				char single[1];
				single[0] = '\n';
				if (write(fd, single, sizeof(single)) == -1){
					errorMe("writing");
				}
			}
		}else{
			if (write(fd, buf+i, 1) == -1){
				errorMe("writing");
			}
		}
	}
}

void twoProcesses(){
	int pollRes;	
	signal(SIGPIPE, sighandler);
	
	// create pipes | pipes are undirectional so we need two
	if (pipe(toChild) == -1 || pipe(fromChild) == -1){ 
		errorMe("creating pipe");
	}
	// fork to create a new process
	if ((pid = fork()) == -1){
        	errorMe("forking");
	}
	else if (pid == 0){ // if child process
        	// exec the shell
                if (close(toChild[1]) == -1 || close(fromChild[0]) == -1){
			errorMe("closing pipe");
		}
		// stdinput is pipe from terminal process, stdoutput and stderror are a pipe to terminal process
		if (dup2(toChild[0], STDIN_FILENO) == -1 || dup2(fromChild[1], STDOUT_FILENO) == -1 || dup2(fromChild[1], STDERR_FILENO) == -1){
			errorMe("duplicating file descriptor");
		}

                if (close(toChild[0]) == -1 || close(fromChild[1]) == -1){
			errorMe("closing pipe");
		} 
		// tribute to the legend
		char *kamp[] = {
			"/bin/bash",
			NULL
		};
		
		if (execvp(kamp[0], kamp) == -1){
			errorMe("using execvp");
		}
	}
	else if (pid > 0){ // parent process
		if (close(toChild[0]) == -1 || close(fromChild[1]) == -1) {
			errorMe("closing pipe");
		}

		struct pollfd pfd[2];
		int readRes;
		
		// have both poll fds wait for either input or error events
		pfd[0].fd = STDIN_FILENO;
		pfd[0].events = POLLIN | POLLHUP | POLLERR;
		pfd[1].fd = fromChild[0];
		pfd[1].events = POLLIN | POLLHUP | POLLERR;

		while(1){
			pollRes = poll(pfd,2,0); // timeout=0
			if (pollRes >= 0){
				if (pollRes == 0) continue; // call timed out and no file descriptors are ready
				if ((pfd[0].revents&POLLIN) == POLLIN){ // only read if it has pending input
					char stdinBuf[256]; 
					if ((readRes = read(STDIN_FILENO, stdinBuf, sizeof(stdinBuf))) == -1){
						errorMe("reading");
					}
					// process input
					writeThat(STDOUT_FILENO, stdinBuf, readRes);
					writeThat(toChild[1], stdinBuf, readRes);
				}

				if ((pfd[1].revents&POLLIN) == POLLIN){
					char shellBuf[256];
					if ((readRes = read(pfd[1].fd, shellBuf, sizeof(shellBuf))) == -1){
						errorMe("reading");
					}
					writeThat(STDOUT_FILENO, shellBuf, readRes);
				}
				
				if (pfd[1].revents & (POLLHUP | POLLERR)){
					close(toChild[1]);
					break;
				}
			}
			else{
				errorMe("using poll");
			}
		}
	}
}			


int
main(int argc, char **argv){
	int c, result;
	int option_index = 0;	
	char buf[256];	

	static struct option long_options[] = {
		{"shell",no_argument,0,'s'},
		{"debug",no_argument,0,'d'},
		{0,0,0,0}
	};
	
	// restore normal terminal modes at exit
	atexit(resetSettings);

	// get current terminal modes
	if(tcgetattr(STDIN_FILENO, &initTerm) == -1) { 
                errorMe("using tcgetattr");
        }

        // make a copy
        if (tcgetattr(STDIN_FILENO, &newTerm) == -1) {
        	errorMe("using tcgetattr");
        }	
	
	while ((c = getopt_long(argc, argv, "sd", long_options, &option_index)) != -1){
		switch(c){
			case 's':
		 		shellFlag = 1;
				break;
			case 'd':
				debugFlag = 1;
				fprintf(stderr, "Beginning debug mode!\n");
				break;
			default:
				// detect bad arguments
				fprintf(stderr, "Invalid option. Correct usage: lab1a [--shell] [--debug]\n");
				exit(1);
		}
	}

        newTerm.c_iflag = ISTRIP; // only lower 7 bits
        newTerm.c_oflag = 0; // no processing
        newTerm.c_lflag = 0; // no processing
        newTerm.c_cc[VTIME] = 0; // inter-char timer unused
        newTerm.c_cc[VMIN] = 1; // block read until 1 char received
        if (tcsetattr(STDIN_FILENO,TCSANOW,&newTerm) == -1) {
		errorMe("using tcsetattr");
	}

	if (shellFlag){
		twoProcesses();
	}
	else{
		// issue read for whatever your buffer size is
		if ((result = read(STDIN_FILENO, buf, sizeof(buf))) == -1){
			errorMe("reading");
		}
		while(result > 0){
			// write received characters back out to display
			writeThat(STDOUT_FILENO, buf, result);
			// issue another read
			if ((result = read(STDIN_FILENO, buf, sizeof(buf))) == -1){
				errorMe("reading");
			}
		}
	}
}
