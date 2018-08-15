// NAME: Konner Macias
// EMAIL: konnermacias@g.ucla.edu
// ID: 004603916


#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<getopt.h>
#include<errno.h>
#include<string.h>
#include<signal.h>
#include<fcntl.h>

// the below two functions were taken by the file descriptor
// management tutorial listed in the spec.
void input_opt(char* newfile)
{
	int ifd = open(newfile, O_RDONLY);
	if (ifd >= 0)
	{
		close(0);
		dup(ifd);
		close(ifd);
	}else{
		printf("Error: %s\n", strerror(errno));
		fprintf(stderr, "Error with input redirection");
		exit(2);
	}
}

void output_opt(char* newfile)
{
	int ofd = creat(newfile, 0666);
	if (ofd >= 0)
	{
		close(1);
		dup(ofd);
		close(ofd);
	}else{
		printf("Error: %s\n", strerror(errno));
		fprintf(stderr, "Error with output redirection");
		exit(3);
	}
}

void sighandler(int signum)
{
	if (signum == SIGSEGV)
	{
		printf("Error: %s", strerror(errno));
		fprintf(stderr, "Segmentation Fault");
		exit(4);
	}
}


int
main(int argc, char **argv)
{

	int c;
	int option_index = 0;
	char* buf[2];
	ssize_t fd;
	int segFlag = 0;	

	static struct option long_options[] = {
	    {"input", required_argument, 0, 'i'},
	    {"output", required_argument, 0, 'o'},
	    {"segfault", no_argument, 0, 's'},
	    {"catch", no_argument, 0, 'c'},
	    {0,	0, 0, 0}
	};
	
	
	while((c = getopt_long(argc, argv, "i:o:sc", long_options, &option_index)) != -1)
	{
		switch(c)
		{
			// use specified file as stdin
			case 'i':
				input_opt(optarg);
				break;
			case 'o':
				output_opt(optarg);
				break;
			case 's':
				segFlag = 1;
				break;
			case 'c':
				signal(SIGSEGV, sighandler);
				break;
			default:
				printf("Error: %s", strerror(errno));
				fprintf(stderr, "Invalid Option. Correct usage: ./lab0 --input file1 --output file2 [--segfault] [--catch]\n");
				exit(1);
		}
	}

	if (segFlag)
	{
		char* var = NULL;
		*var = 'k';
	}

	while((fd = read(STDIN_FILENO, &buf,1)) > 0)
	{
		if (fd == -1)
		{
			printf("Error: %s", strerror(errno));
			fprintf(stderr, "Cannot read in from stdin!");
			exit(1);

		}

		if (write(STDOUT_FILENO, &buf, fd) == -1)
		{
			printf("Error: %s", strerror(errno));
			fprintf(stderr, "Cannot write to stdout!");
			exit(1);
		}
	}

	exit(0);
}			
