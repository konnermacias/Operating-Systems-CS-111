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
#include "zlib.h"
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

struct termios initTerm, newTerm;
pid_t pid;
int portnum, sockfd;
z_stream toServer, fromServer;
int comFlag = 0;
int logFd = -1;

void errorMe(char *str) {
	fprintf(stderr, "Error %s: %s\n", str, strerror(errno));
	exit(1);
}

void resetSettings() {
	if (tcsetattr(STDIN_FILENO, TCSANOW, &initTerm) == -1) {
		errorMe("resetting terminal settings");
	}
}

void stopStreams() {
	deflateEnd(&toServer);
	inflateEnd(&fromServer);
}

void zNullify(z_stream *z) {
	z->zalloc = Z_NULL;
	z->zfree = Z_NULL;
	z->opaque = Z_NULL;
}


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

void writeThat(int fd, char *buf, int numBytes) {
	int i;
	char c;
	for (i = 0; i < numBytes; i++) {
		c = buf[i];
		if (c == 0x0D || c == 0x0A) {
			if (fd == STDOUT_FILENO) {	
				char special[2];
				special[0] = '\r';
				special[1] = '\n';
				if (write(fd, special, sizeof(special)) == -1){
					errorMe("writing");
				}
			}else{
				/* to the socket */
				char single[1];
				single[0] = '\n';
				if (write(fd, single, sizeof(single)) == -1){
					errorMe("writing");
				}
			}
		}else{
			if (write(fd, buf+i, 1) == -1) {
				errorMe("writing");
			}
		}
	}
}

void socketConnect() {
	struct sockaddr_in serv_addr;
	/* create a socket point */
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
                errorMe("opening socket");
        }

        struct hostent *server = gethostbyname("localhost");

        memset((char*) &serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(portnum);
        memcpy((char*) &serv_addr.sin_addr.s_addr, (char *) server->h_addr, server->h_length);

        /* Now connect to the server */
        if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
                errorMe("connecting");
        }
}

void terminalMode() {
	atexit(resetSettings);
	if (tcgetattr(STDIN_FILENO, &initTerm) == -1) {
                errorMe("using tcgetattr");
        }
	
        if (tcgetattr(STDIN_FILENO, &newTerm) == -1) {
                errorMe("using tcgetattr");
        }

        newTerm.c_iflag = ISTRIP;
        newTerm.c_oflag = 0;
        newTerm.c_lflag = 0;

        if (tcsetattr(STDIN_FILENO, TCSANOW, &newTerm) == -1) {
                errorMe("using tcsetattr");
        }
}

void setUpCompression() {
	/* set up compression */
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
	int portFlag = 0;
	char *log_file;
	int option_index = 0;
	int logFlag = 0;

	static struct option long_options[] = {
		{"port", required_argument, 0, 'p'},
		{"log", optional_argument, 0, 'l'},
		{"compress", no_argument, 0, 'c'},
		{0,0,0,0}
	};

	while ((c = getopt_long(argc, argv, "p:l:c", long_options, &option_index)) != -1) {
		switch(c) {
			case 'p':
				portnum = atoi(optarg);
		 		portFlag = 1;
				break;
			case 'l':
				log_file = optarg;
				logFlag = 1;
				break;
			case 'c':
				comFlag = 1;
				break;
			default:
				fprintf(stderr, "Invalid option. Correct usage: lab1b  --port=port_num [--log=log_file] [--compress]\n");
				exit(1);
		}
	}

	if (!portFlag) {
		fprintf(stderr, "Please specify port number. Correct usage: lab1b --port=port_num [--log=logfile] [--compress]\n");
		exit(1);
	}
	if (logFlag) {
		if ((logFd = creat(log_file, S_IRWXU)) < 0) {
			errorMe("opening or creating logfile");
		}
	}
		
	if (comFlag) {
		setUpCompression();
	}

	terminalMode();
	socketConnect();	
	
	struct pollfd pfd[2];
	int pollRes, readRes;
	
	pfd[0].fd = STDIN_FILENO;
	pfd[0].events = POLLIN | POLLHUP | POLLERR;
	pfd[1].fd = sockfd;
	pfd[1].events = POLLIN | POLLHUP | POLLERR;

	while(1) {
		pollRes = poll(pfd, 2, 0);
		if (pollRes == 0) continue;
		if (pollRes < 0) {
			errorMe("polling");
		}
		/* check events */
		if ((pfd[0].revents&POLLIN) == POLLIN) { 
			char stdinBuf[256];	
			if ((readRes = read(STDIN_FILENO, stdinBuf, sizeof(stdinBuf))) == -1) {
				errorMe("reading");
			}
			writeThat(STDOUT_FILENO, stdinBuf, readRes);

			/* transform all \r to \n */
			for (int j = 0; j < readRes; j++) {
				if (stdinBuf[j] == '\r') {
					stdinBuf[j] = '\n';
				}
			}

			/* compress */
			if (comFlag) {
				readRes = press(stdinBuf, readRes, 1);
			}

			if (write(pfd[1].fd, stdinBuf, readRes) < 0) {
				errorMe("using write");
			}

			/* show sent compressed bytes */
			if (logFd >=  0) {
				if (dprintf(logFd, "SENT %d bytes: ", readRes) < 0) {
                                	errorMe("using dprintf");
                                }
                                if (write(logFd, stdinBuf, readRes) < 0) {
                                	errorMe("writing");
                                } 
                                if (dprintf(logFd, "\n") < 0) {
                                	errorMe("using dprintf");
                                }
			}
		}
		if ((pfd[1].revents&POLLIN) == POLLIN) {
			char serverBuf[256];
			if ((readRes = read(pfd[1].fd, serverBuf, sizeof(serverBuf))) == -1) {
				errorMe("reading");
			}
			else if (readRes == 0) {
				if (close(sockfd) < 0) {
					errorMe("closing socket");
				}
				exit(0);
			}

			if (logFd >= 0) {
                                if (dprintf(logFd, "RECEIVED %d bytes: ", readRes) < 0) {
                                        errorMe("using dprintf");
                                }
				
                                if (write(logFd, serverBuf, readRes) < 0) {
                                        errorMe("writing");
                                }        
                                if (dprintf(logFd, "\n") < 0) {
                                        errorMe("using dprintf");
                                }
                        }
	
			if (comFlag) {
				readRes = press(serverBuf, readRes, 0);
			}
			
			writeThat(STDOUT_FILENO, serverBuf, readRes);
		}	
		if (pfd[1].revents & (POLLHUP | POLLERR)) {
			if (close(sockfd) == -1) {
				errorMe("close failed");
			}
			exit(0);
		}
	}

	exit(0);
}

