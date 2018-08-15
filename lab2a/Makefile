# NAME: Konner Macias
# EMAIL: konnermacias@g.ucla.edu
# ID: 004603916

my_code = lab2_add.c SortedList.h SortedList.c lab2_list.c
my_data = lab2_add.csv lab2_list.csv
my_gp = lab2_add.gp lab2_list.gp
png_add = lab2_add-1.png lab2_add-2.png lab2_add-3.png lab2_add-4.png lab2_add-5.png
png_list = lab2_list-1.png lab2_list-2.png lab2_list-3.png lab2_list-4.png

CC = gcc

default: build

build:
	$(CC) -Wall -Wextra -pthread -o lab2_add lab2_add.c 
	$(CC) -Wall -Wextra -pthread -o lab2_list lab2_list.c SortedList.c

graphs: tests
	@chmod +x ./lab2_add.gp
	@chmod +x ./lab2_list.gp
	gnuplot ./lab2_add.gp
	gnuplot ./lab2_list.gp

dist: graphs
	tar -czf lab2a-004603916.tar.gz Makefile README $(my_code) $(my_data) $(png_add) $(png_list) $(my_gp)

clean:
	rm -f lab2a-004603916.tar.gz lab2_add lab2_list

tests: clean build
	rm -f lab2_add.csv
	rm -f lab2_list.csv

	# I ran for (1,2,4,8,12) threads and (10,20,40,80,100,1000,
	# 10000, 100000, 1000000) iterations with and without yields
	./lab2_add --threads=1  --iterations=10      >> lab2_add.csv
	./lab2_add --threads=1  --iterations=20      >> lab2_add.csv
	./lab2_add --threads=1  --iterations=40      >> lab2_add.csv
	./lab2_add --threads=1  --iterations=80      >> lab2_add.csv
	./lab2_add --threads=1  --iterations=100     >> lab2_add.csv
	./lab2_add --threads=1  --iterations=1000    >> lab2_add.csv
	./lab2_add --threads=1  --iterations=10000   >> lab2_add.csv
	./lab2_add --threads=1  --iterations=100000  >> lab2_add.csv
	./lab2_add --threads=1  --iterations=1000000 >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10      >> lab2_add.csv
	./lab2_add --threads=2  --iterations=20      >> lab2_add.csv
	./lab2_add --threads=2  --iterations=40      >> lab2_add.csv
	./lab2_add --threads=2  --iterations=80      >> lab2_add.csv
	./lab2_add --threads=2  --iterations=100     >> lab2_add.csv
	./lab2_add --threads=2  --iterations=1000    >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10000   >> lab2_add.csv
	./lab2_add --threads=2  --iterations=100000  >> lab2_add.csv
	./lab2_add --threads=2  --iterations=1000000 >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10      >> lab2_add.csv
	./lab2_add --threads=4  --iterations=20      >> lab2_add.csv
	./lab2_add --threads=4  --iterations=40      >> lab2_add.csv
	./lab2_add --threads=4  --iterations=80      >> lab2_add.csv
	./lab2_add --threads=4  --iterations=100     >> lab2_add.csv
	./lab2_add --threads=4  --iterations=1000    >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000   >> lab2_add.csv
	./lab2_add --threads=4  --iterations=100000  >> lab2_add.csv
	./lab2_add --threads=4  --iterations=1000000 >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10     >> lab2_add.csv
	./lab2_add --threads=8  --iterations=20     >> lab2_add.csv
	./lab2_add --threads=8  --iterations=40     >> lab2_add.csv
	./lab2_add --threads=8  --iterations=80     >> lab2_add.csv
	./lab2_add --threads=8  --iterations=100    >> lab2_add.csv
	./lab2_add --threads=8  --iterations=1000   >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10000  >> lab2_add.csv
	./lab2_add --threads=8  --iterations=100000 >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10     >> lab2_add.csv
	./lab2_add --threads=12 --iterations=20     >> lab2_add.csv
	./lab2_add --threads=12 --iterations=40     >> lab2_add.csv
	./lab2_add --threads=12 --iterations=80     >> lab2_add.csv
	./lab2_add --threads=12 --iterations=100    >> lab2_add.csv
	./lab2_add --threads=12 --iterations=1000   >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10000  >> lab2_add.csv
	./lab2_add --threads=12 --iterations=100000 >> lab2_add.csv
	./lab2_add --yield --threads=1  --iterations=10      >> lab2_add.csv
	./lab2_add --yield --threads=1  --iterations=20      >> lab2_add.csv
	./lab2_add --yield --threads=1  --iterations=40      >> lab2_add.csv
	./lab2_add --yield --threads=1  --iterations=80      >> lab2_add.csv
	./lab2_add --yield --threads=1  --iterations=100     >> lab2_add.csv
	./lab2_add --yield --threads=1  --iterations=1000    >> lab2_add.csv
	./lab2_add --yield --threads=1  --iterations=10000   >> lab2_add.csv
	./lab2_add --yield --threads=1  --iterations=100000  >> lab2_add.csv
	./lab2_add --yield --threads=1  --iterations=1000000 >> lab2_add.csv
	./lab2_add --yield --threads=2  --iterations=10      >> lab2_add.csv
	./lab2_add --yield --threads=2  --iterations=20      >> lab2_add.csv
	./lab2_add --yield --threads=2  --iterations=40      >> lab2_add.csv
	./lab2_add --yield --threads=2  --iterations=80      >> lab2_add.csv
	./lab2_add --yield --threads=2  --iterations=100     >> lab2_add.csv
	./lab2_add --yield --threads=2  --iterations=1000    >> lab2_add.csv
	./lab2_add --yield --threads=2  --iterations=10000   >> lab2_add.csv
	./lab2_add --yield --threads=2  --iterations=100000  >> lab2_add.csv
	./lab2_add --yield --threads=2  --iterations=1000000 >> lab2_add.csv
	./lab2_add --yield --threads=4  --iterations=10      >> lab2_add.csv
	./lab2_add --yield --threads=4  --iterations=20      >> lab2_add.csv
	./lab2_add --yield --threads=4  --iterations=40      >> lab2_add.csv
	./lab2_add --yield --threads=4  --iterations=80      >> lab2_add.csv
	./lab2_add --yield --threads=4  --iterations=100     >> lab2_add.csv
	./lab2_add --yield --threads=4  --iterations=1000    >> lab2_add.csv
	./lab2_add --yield --threads=4  --iterations=10000   >> lab2_add.csv
	./lab2_add --yield --threads=4  --iterations=100000  >> lab2_add.csv
	./lab2_add --yield --threads=4  --iterations=1000000 >> lab2_add.csv
	./lab2_add --yield --threads=8  --iterations=10     >> lab2_add.csv
	./lab2_add --yield --threads=8  --iterations=20     >> lab2_add.csv
	./lab2_add --yield --threads=8  --iterations=40     >> lab2_add.csv
	./lab2_add --yield --threads=8  --iterations=80     >> lab2_add.csv
	./lab2_add --yield --threads=8  --iterations=100    >> lab2_add.csv
	./lab2_add --yield --threads=8  --iterations=1000   >> lab2_add.csv
	./lab2_add --yield --threads=8  --iterations=10000  >> lab2_add.csv
	./lab2_add --yield --threads=8  --iterations=100000 >> lab2_add.csv
	./lab2_add --yield --threads=12 --iterations=10     >> lab2_add.csv
	./lab2_add --yield --threads=12 --iterations=20     >> lab2_add.csv
	./lab2_add --yield --threads=12 --iterations=40     >> lab2_add.csv
	./lab2_add --yield --threads=12 --iterations=80     >> lab2_add.csv
	./lab2_add --yield --threads=12 --iterations=100    >> lab2_add.csv
	./lab2_add --yield --threads=12 --iterations=1000   >> lab2_add.csv
	./lab2_add --yield --threads=12 --iterations=10000  >> lab2_add.csv
	./lab2_add --yield --threads=12 --iterations=100000 >> lab2_add.csv

	# With yields, run for (2,4,8,12) threads at 1000 iterations for (m,c,s) for
	# the different synchronization options with & w/o yields
	./lab2_add --threads=2  --iterations=10000 --yield --sync=m >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000 --yield --sync=m >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10000 --yield --sync=m >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10000 --yield --sync=m >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10000 --yield --sync=c >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000 --yield --sync=c >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10000 --yield --sync=c >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10000 --yield --sync=c >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10000 --yield --sync=s >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000 --yield --sync=s >> lab2_add.csv
	./lab2_add --threads=8  --iterations=1000  --yield --sync=s >> lab2_add.csv
	./lab2_add --threads=12 --iterations=1000  --yield --sync=s >> lab2_add.csv
	./lab2_add --threads=1  --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=1  --iterations=10000 --sync=c >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10000 --sync=c >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000 --sync=c >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10000 --sync=c >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10000 --sync=c >> lab2_add.csv
	./lab2_add --threads=1  --iterations=10000 --sync=s >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10000 --sync=s >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000 --sync=s >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10000 --sync=s >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10000 --sync=s >> lab2_add.csv

	# Lists #
	# basic runs for 1 thread at (10,100,1000,10000,20000) iterations
	./lab2_list --threads=1  --iterations=10	>> lab2_list.csv
	./lab2_list --threads=1  --iterations=100   >> lab2_list.csv
	./lab2_list --threads=1  --iterations=1000  >> lab2_list.csv
	./lab2_list --threads=1  --iterations=10000 >> lab2_list.csv
	./lab2_list --threads=1  --iterations=20000	>> lab2_list.csv
	
	# For multiple threads (2,4,8,12) at (1,10,100,1000) iterations
	# these should fail so I put a -
	-./lab2_list --threads=2  --iterations=1	>> lab2_list.csv
	-./lab2_list --threads=2  --iterations=10   >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=100  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=1000 >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=1    >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=10   >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=100  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=1000 >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=1    >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=10   >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=100  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=1000	>> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1    >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=10   >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=100  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1000 >> lab2_list.csv
	
	# for (2,4,8,12) threads and (i,d,il,dl) yield options at 
	# (1,2,4,8,16,32) iterations
	-./lab2_list --threads=2  --iterations=1  --yield=i >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=2  --yield=i >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=4  --yield=i >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=8  --yield=i >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=16 --yield=i >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=32 --yield=i >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=1  --yield=i >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=2  --yield=i >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=4  --yield=i >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=8  --yield=i >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=16 --yield=i >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=1  --yield=i >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=2  --yield=i >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=4  --yield=i >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=8  --yield=i >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=16 --yield=i >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1  --yield=i >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=2  --yield=i >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=4  --yield=i >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=8  --yield=i >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=16 --yield=i >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=1  --yield=d >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=2  --yield=d >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=4  --yield=d >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=8  --yield=d >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=16 --yield=d >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=32 --yield=d >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=1  --yield=d >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=2  --yield=d >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=4  --yield=d >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=8  --yield=d >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=16 --yield=d >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=1  --yield=d >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=2  --yield=d >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=4  --yield=d >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=8  --yield=d >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=16 --yield=d >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1  --yield=d >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=2  --yield=d >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=4  --yield=d >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=8  --yield=d >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=16 --yield=d >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=1  --yield=il >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=2  --yield=il >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=4  --yield=il >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=8  --yield=il >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=16 --yield=il >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=32 --yield=il >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=1  --yield=il >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=2  --yield=il >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=4  --yield=il >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=8  --yield=il >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=16 --yield=il >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=1  --yield=il >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=2  --yield=il >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=4  --yield=il >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=8  --yield=il >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=16 --yield=il >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1  --yield=il >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=2  --yield=il >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=4  --yield=il >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=8  --yield=il >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=16 --yield=il >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=1  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=2  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=4  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=8  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=16 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=32 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=1  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=2  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=4  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=8  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=16 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=1  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=2  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=4  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=8  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=16 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=2  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=4  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=8  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=16 --yield=dl >> lab2_list.csv

	# run for 12 threads with (i,d,il,dl) yield options and (m,s) sync options
	# at 32 iterations each
	./lab2_list --threads=12 --iterations=32 --yield=i  --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=d  --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=i  --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=d  --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=dl --sync=s >> lab2_list.csv

	# run for (1,2,4,8,12,16,24) threads at 1000 iterations for (m,s) sync options
	./lab2_list --threads=1  --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=1  --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=s >> lab2_list.csv
