# NAME: Konner Macias
# EMAIL: konnermacias@g.ucla.edu
# ID: 004603916


my_files = lab1a.c README Makefile
CC = gcc
CFLAGS = -Wall -Wextra

default:
	$(CC) $(CFLAGS) -o lab1a lab1a.c

dist: clean default
	tar -czf lab1a-004603916.tar.gz $(my_files)

clean:
	@rm -f lab1a lab1a-004603916.tar.gz
