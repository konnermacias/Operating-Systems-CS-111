# NAME: Konner Macias
# EMAIL: konnermacias@g.ucla.edu
# ID: 004603916

my_code = lab4b.c

CC = gcc

default: build


build:
	$(CC) -Wall -Wextra -g -lmraa -o lab4b lab4b.c -lm


check: clean default smoke_tests
	if [ -e errors.txt ]; then \
		cat errors.txt; \
		rm errors.txt; \
	else \
		echo "PASSED ALL TESTS" ;\
	fi


clean:
	rm -f lab4b-004603916.tar.gz lab4b


dist: clean
	tar -czf lab4b-004603916.tar.gz Makefile README lab4b.c


smoke_tests: testSuccess bad_argument 

testSuccess:	
	echo "PERIOD=1\nSCALE=F\nSTART\nSTOP\nLOG YO\nOFF" | ./lab4b --scale=C --log=LOGFILE --period=2; \
	if [ $$? -ne 0 ] ; then \
		echo "TEST FAILED: CORRECT OPTIONS YIELD INCORRECT EXIT CODE" >> errors.txt; \
	fi


bad_argument:
	./lab4b --fewfwC ; \
	if [ $$? -ne 1 ]; then \
		echo "TEST FAILED: WRONG EXIT CODE FOR BAD ARG" >> errors.txt; \
	fi
