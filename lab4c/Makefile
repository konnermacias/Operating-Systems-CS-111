# NAME: Konner Macias
# EMAIL: konnermacias@g.ucla.edu
# ID: 004603916

my_code = lab4c.c

CC = gcc

default: build


build:
	$(CC) -Wall -Wextra -g -lmraa -o lab4c_tcp lab4c_tcp.c -lm
	$(CC) -Wall -Wextra -g -lssl -lcrypto -lmraa -o lab4c_tls lab4c_tls.c -lm

dist:
	tar -czf lab4c-004603916.tar.gz lab4c_tcp.c lab4c_tls.c Makefile README

clean:
	rm -f lab4c-004603916.tar.gz lab4c_tcp lab4c_tls
