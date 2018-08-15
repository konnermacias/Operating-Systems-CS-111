# NAME: Konner Macias
# EMAIL: konnermacias@g.ucla.edu
# ID: 004603916

my_code = SortedList.h SortedList.c lab2_list.c
my_data = lab2b_list.csv
my_gp = lab2b_list.gp
png_list = lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png

CC = gcc

default: build

build:
	$(CC) -Wall -Wextra -g -pthread -lprofiler -o lab2_list lab2_list.c SortedList.c

profile: build
	rm -f ./raw.gperf
	LD_PRELOAD=~/Documents/CS_111/lib/libprofiler.so.0 CPUPROFILE=./raw.gperf ./lab2_list --threads=12 --iterations=1000 --sync=s
	~/Documents/CS_111/bin/pprof --text ./lab2_list ./raw.gperf > profile.out
	~/Documents/CS_111/bin/pprof --list=tFunc ./lab2_list ./raw.gperf >> profile.out
	rm -f ./raw.gperf	

graphs: tests
	@chmod +x ./lab2b_list.gp
	gnuplot ./lab2b_list.gp

dist: graphs
	tar -czf lab2b-004603916.tar.gz Makefile README profile.out $(my_code) $(my_data) $(png_list) $(my_gp)

clean:
	rm -f lab2b-004603916.tar.gz lab2_list

tests: clean build
	rm -f lab2b_list.csv
	
	./lab2_list --threads=1 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=s >> lab2b_list.csv
	# for (1,4,8,12,16) threads and (1,2,4,8,16) iterations with the
	# --yield option and no synchronization
	-./lab2_list --threads=1 --iterations=1 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=2 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=16 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=1 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=2 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=16 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=1 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=2 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=16 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=1 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=2 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=16 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=1 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=2 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=16 --yield=id --lists=4 >> lab2b_list.csv
	# Now with --sync=s and --sync=m
	-./lab2_list --threads=1 --iterations=10 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=20 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=40 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=80 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=10 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=20 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=40 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=80 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=10 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=20 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=40 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=80 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=10 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=20 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=40 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=80 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=10 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=20 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=40 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=80 --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=10 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=20 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=40 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=80 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=10 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=20 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=40 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=80 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=10 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=20 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=40 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=80 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=10 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=20 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=40 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=80 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=10 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=20 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=40 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=80 --yield=id --lists=4 --sync=m >> lab2b_list.csv
	# run sync without yields for 1000 iterations (1,2,4,8,12) threads and (1,4,8,16) lists
	./lab2_list --threads=1 --iterations=1000 --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv

