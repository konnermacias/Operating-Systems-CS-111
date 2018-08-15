# NAME: Konner Macias
# EMAIL: konnermacias@g.ucla.edu
# ID: 004603916


my_files = lab0.c README Makefile backtrace.png breakpoint.png
CC = gcc
CFLAGS = -Wall -Wextra

default:
	$(CC) $(CFLAGS) -o lab0 lab0.c -g

check: clean default smoke_tests
	if [[ -s errors.txt ]]; then \
		cat errors.txt; \
		rm errors.txt; \
	fi

dist: clean default
	tar -czf lab0-004603916.tar.gz $(my_files)

clean:
	@rm -f lab0 input.txt output.txt lab0-004603916.tar.gz


smoke_tests: testSuccess testInvalidArg testCatch testInput testCreat

testSuccess:
	echo "Konner" > input.txt; \
	./lab0 --input input.txt --output output.txt; \
	
	if [[$$? -ne 0]]; then \
		echo "Error handling --input and --output" >> errors.txt; \
	fi; \
	
	diff input.txt output.txt; \
	if [[$$? -ne 0]]; then \
		echo "Error! Contents of input and output are not the same" >> errors.txt; \
	fi; \
	
	@rm -f input.txt output.txt

testInvalidArg:
	echo "Konner" > input.txt; \
	./lab0 --input input.txt --output output.txt --segfault --gibberish; \
	if [[[$$? -ne 2]]; then \
		echo "Error! Did not handle unrecognized argument correctly" >> errors.txt; \
	fi; \
	
	@rm -f input.txt

testCatch:
	echo "Konner" > input.txt; \
	./lab0 --input input.txt --output output.txt --segfault --catch; \
	if [[$$? -ne 4]]; then \
		echo "Error! Did not catch the segmentation fault" >> errors.txt; \
	fi; \
	
	@rm -f input.txt output.txt

testInput:
	./lab0 --input missing.txt --output output.txt; \
	if [[$$? -ne 2]]; then \
		echo "Error handling non-existant input file" >> errors.txt; \
	fi; \
	
	echo "Konner" > input.txt; \
	chmod u-r input.txt; \
	./lab0 --input input.txt --output output.txt; \
	if [[$$? -ne 2]]; then \
		echo "Error handling unreadable input file" >> errors.txt; \
	fi; \

	@rm -f input.txt output.txt
	

testCreat:
	echo "Konner" > input.txt; \
	touch output.txt; \
	chmod u-w output.txt; \
	./lab0 --input input.txt --output output.txt; \
	if [[$$? -ne 3]]; then \
		echo "Error handling uncreatable output file" >> errors.txt; \
	fi; \
	
	@rm -f input.txt output.txt


