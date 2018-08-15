# NAME: Konner Macias
# EMAIL: konnermacias@g.ucla.edu
# ID: 004603916

my_files = lab1b-server.c lab1b-client.c README Makefile
CC = gcc

default:
	$(CC) -Wall -Wextra -lz -o lab1b-server lab1b-server.c
	$(CC) -Wall -Wextra -lz -o lab1b-client lab1b-client.c

dist: clean default
	tar -czf lab1b-004603916.tar.gz $(my_files)

clean:
	@rm -f lab1b-004603916.tar.gz lab1b-server lab1b-client

	
