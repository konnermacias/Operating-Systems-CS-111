// NAME: Konner Macias
// EMAIL: konnermacias@g.ucla.edu
// ID: 004603916

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <termios.h>
#include <getopt.h>
#include <fcntl.h>
#include <pthread.h>
#include <poll.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "zlib.h"

int toChild[2];
int fromChild[2];
pid_t pid = -1;
int comFlag = 0;
int portFlag = 0;
int portnum, newsockfd;
z_stream toServer, fromServer;

struct pollfd pfd[2];

void errorMe(char *str) {
	fprintf(stderr, "Error %s: %s\n", str, strerror(errno));
	exit(1);
}

/* used to clean up child process */
void cleanMe() {
        int val;
        pid_t waitForIt;
        if ((waitForIt = waitpid(pid, &val, 0)) == -1) {
                errorMe("using waitpid");
        }
        fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WTERMSIG(val), WEXITSTATUS(val));
        exit(0);
}


void sighandler(int signum) {
	if (signum == SIGPIPE) {
		cleanMe();
	}
}


/* upon exit, make sure to stop streams */
void stopStreams() {
        inflateEnd(&fromServer);
        deflateEnd(&toServer);
}


/* use default memory allocation routines */
void zNullify(z_stream *z) {
        z->zalloc = Z_NULL;
        z->zfree = Z_NULL;
        z->opaque = Z_NULL;
}


/* used to compress or decompress data */
int press(char* buf, int readRes, int compress) {
        int numBytes;
        char tmpBuf[256];
        memcpy(tmpBuf, buf, readRes);

        if (compress) {
                
		toServer.avail_in = readRes;
                toServer.next_in = (Bytef *) tmpBuf;
                toServer.avail_out = 256;
                toServer.next_out = (Bytef *) buf;

                do {
                        deflate(&toServer, Z_SYNC_FLUSH);
                } while (toServer.avail_in > 0);

                numBytes = 256 - toServer.avail_out;
        }else{
		
                fromServer.avail_in = readRes;
                fromServer.next_in = (Bytef *) tmpBuf;
                fromServer.avail_out = 256;
                fromServer.next_out = (Bytef *) buf;
                do {
                        inflate(&fromServer, Z_SYNC_FLUSH);
                } while (fromServer.avail_in > 0);

                numBytes = 256 - fromServer.avail_out;
        }
        return numBytes;
}


void writeThat(int fd, char *buf, int numBytes, int to_Child) {
        int i;
        char c;
        for (i = 0; i < numBytes; i++) {
                c = buf[i];
                if (c == 0x03) {
                	/* ^C from Client sends SIGINT to shell */
			if (to_Child) {
				if (kill(pid, SIGINT) == -1) {
					errorMe("using kill");
				}
			}
		}
		if (c == 0x04) {
			/* ^D from client closes connection */
			if (to_Child) {
				close(toChild[1]);
			}      
		}else {
			if (write(fd, buf+i, 1) == -1) {
                                errorMe("writing");
                        }
                }
        }
}


void socketClient() {
	struct sockaddr_in serv_addr, cli_addr;
	/* first call to socket() */
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (sockfd < 0) {
                errorMe("opening socket");
        }

        /* initialize socket structure */
        bzero((char*) &serv_addr, sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(portnum);

        /* bind the host address */
        if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
                errorMe("on binding");
        }

        /* Now we listen to 1 client */
        if (listen(sockfd, 1) == -1) {
                errorMe("using listen");
        }

        unsigned int clilen = sizeof(cli_addr);

        /* accept actual connection from client */
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) {
                errorMe("on accept");
        }
}

/* create child shell process */
void shellProcess() {

        if (signal(SIGPIPE, sighandler) == SIG_ERR) {
                errorMe("using signal");
        }

        if (pipe(toChild) == -1 || pipe(fromChild) == -1) {
                errorMe("creating pipe");
        }

        if ((pid = fork()) == -1) {
                errorMe("forking");
        }
        else if (pid == 0) {
                if (close(toChild[1]) == -1 ||
                    close(fromChild[0]) == -1){
                        errorMe("closing pipe");
                }

                if ((dup2(toChild[0],    STDIN_FILENO) == -1) ||
                    (dup2(fromChild[1], STDOUT_FILENO) == -1)  ||
                    (dup2(fromChild[1], STDERR_FILENO) == -1 )) {
                        errorMe("duplicating file descriptor");
                }

                if (close(toChild[0]) == -1 ||
                    close(fromChild[1]) == -1) {
                        errorMe("closing pipe");
                }

                char *kamp[] = {
                        "/bin/bash",
                        NULL
                };

                if (execvp(kamp[0], kamp) == -1) {
                        errorMe("using execvp");
                }
        }
        else if (pid > 0) {
                if (close(toChild[0]) == -1 || 
		    close(fromChild[1]) == -1) {
                        errorMe("closing pipe");
                }		
        }
}

/* use if comFlag is set */
void setUpCompression() {
	zNullify(&toServer);
        if (deflateInit(&toServer, Z_DEFAULT_COMPRESSION) < 0) {
        	errorMe("initializing compression stream to server");
        }
        
        zNullify(&fromServer);
        if (inflateInit(&fromServer) < 0) {
  	      errorMe("initalizing decompression stream from server");
        }
        
	atexit(stopStreams);
}


int main(int argc, char **argv) {
	int c;
	int option_index = 0;

        static struct option long_options[] = {
                {"port", required_argument, 0, 'p'},
		{"compress", no_argument, 0, 'c'},
                {0,0,0,0}
        };

	while ((c = getopt_long(argc, argv, "p:c", long_options, &option_index)) != -1) {
                switch(c) {
                        case 'p':
                                portnum = atoi(optarg);
                                portFlag = 1;
                                break;
                        case 'c':
                                comFlag = 1;
                                break;
                        default:
                                fprintf(stderr, "Invalid option. Correct usage: lab1b  --port=portnum [--compress]\n");
                                exit(1);
                }
        }

	if (!portFlag) {
                fprintf(stderr, "Please specify port number. Correct usage: lab1b --port=port_num [--log=logfile] [--compress]\n");
                exit(1);
        }
	
	if (comFlag) {
		setUpCompression();
	}

	socketClient();
	shellProcess();
		
	/* poll keyboard & terminal and shell */
	struct pollfd pfd[2];
	int pollRes, readRes;

	pfd[0].fd = newsockfd;
	pfd[0].events = POLLIN | POLLHUP | POLLERR;
	pfd[1].fd = fromChild[0];
	pfd[1].events = POLLIN | POLLHUP | POLLERR;
	
	if (signal(SIGPIPE, sighandler) == SIG_ERR) {
		errorMe("using signal");
	}	

	while(1) {
		pollRes = poll(pfd, 2, 0);
		if (pollRes == 0) continue;
		if (pollRes < 0) {
			errorMe("polling");
		}
 		/* handle events */
		if ((pfd[0].revents&POLLIN) == POLLIN) {
			char socketBuf[256]; 
			if ((readRes = read(newsockfd, socketBuf, sizeof(socketBuf))) == -1) {
				if (kill(pid, SIGTERM) == -1) {
					errorMe("using kill");
				}
				cleanMe();
			}
			
			if (comFlag) {
				/* decompress */
				readRes = press(socketBuf, readRes, 0);
			}
			writeThat(toChild[1], socketBuf, readRes, 1);
		}
		if ((pfd[1].revents&POLLIN) == POLLIN) {
			char shellBuf[256];
			if ((readRes = read(fromChild[0], shellBuf, sizeof(shellBuf))) == -1) {
				errorMe("reading");
			}
			else if (readRes == 0) {
                               	cleanMe();
			}
			
			if (comFlag) {
				/* compress */
				readRes = press(shellBuf, readRes, 1);
			}

			writeThat(newsockfd, shellBuf, readRes, 0);
		}
				
		if (pfd[1].revents & (POLLHUP | POLLERR)) {
			if (close(pfd[1].fd) == -1) {
				errorMe("using close");
			}
			cleanMe();
		}
	}	
}
