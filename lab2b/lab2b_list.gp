#! /usr/local/cs/bin/gnuplot
#
# purpose:
#	 generate data reduction graphs for the multi-threaded list project
#
# input: lab2_list.csv
#	1. test name
#	2. # threads
#	3. # iterations per thread
#	4. # lists
#	5. # operations performed (threads x iterations x (ins + lookup + delete))
#	6. run time (ns)
#	7. run time per operation (ns)
#
# output:
#	lab2_list-1.png ... cost per operation vs threads and iterations
#	lab2_list-2.png ... threads and iterations that run (un-protected) w/o failure
#	lab2_list-3.png ... threads and iterations that run (protected) w/o failure
#	lab2_list-4.png ... cost per operation vs number of threads
#
# Note:
#	Managing data is simplified by keeping all of the results in a single
#	file.  But this means that the individual graphing commands have to
#	grep to select only the data they want.
#
#	Early in your implementation, you will not have data for all of the
#	tests, and the later sections may generate errors for missing data.
#

# general plot parameters
set terminal png
set datafile separator ","

# how many threads/iterations we can run without failure (w/o yielding)
set title "List-1: Throughput vs. Number of Threads"
set xlabel "Threads"
set logscale x 2
unset xrange
set xrange [0.75:*]
set ylabel "Throughput (op/sec)"
set logscale y 10
set output 'lab2b_1.png'

# grep out only threaded, 1000 iteratoins,protected, non-yield results
plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'Mutex' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'Spin-Lock' with linespoints lc rgb 'green'

set title "List-2: Mean Time/Mutex wait and time/op vs. Number of Threads"
set xlabel "Threads"
set logscale x 2
unset xrange
set xrange [0.75:*]
set ylabel "Time per Operation (sec/op)"
set logscale y 10
set output 'lab2b_2.png'

# grep out only threaded, 1000 iterationns,protected, non-yield results
plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($8) \
        title 'Wait-for-lock' with linespoints lc rgb 'red', \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($7) \
        title 'Avg Time per Op' with linespoints lc rgb 'green'

 
set title "List-3: Protected Iterations that run without failure"
unset logscale x
set xrange [0.75:*]
set xlabel "Threads"
set ylabel "successful iterations"
set logscale y 10
set output 'lab2b_3.png'
plot \
    "< grep 'list-id-m,' lab2b_list.csv" using ($2):($3) \
	with points lc rgb "red" title "Mutex", \
    "< grep 'list-id-s,' lab2b_list.csv" using ($2):($3) \
	with points lc rgb "blue" title "Spin-Lock"


set title "List-4: Throughput of Mutex Protected Lists vs. Threads"
set xlabel "Threads"
set logscale x 2
unset xrange
set xrange [0.75:*]
set ylabel "Throughput (op/sec)"
set logscale y
set output 'lab2b_4.png'
set key right top
plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
        title 'lists=1' with linespoints lc rgb 'red', \
     "< grep 'list-none-m,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
        title 'lists=4' with linespoints lc rgb 'blue', \
     "< grep 'list-none-m,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
        title 'lists=8' with linespoints lc rgb 'green', \
     "< grep 'list-none-m,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
        title 'lists=16' with linespoints lc rgb 'black'


set title "List-5: Throughput of Spin-Lock Protected Lists vs. Threads"
set xlabel "Threads"
set logscale x 2
unset xrange
set xrange [0.75:*]
set ylabel "Throughput (op/sec)"
set logscale y
set output 'lab2b_5.png'
set key right top
plot \
     "< grep 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
        title 'lists=1' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
        title 'lists=4' with linespoints lc rgb 'blue', \
     "< grep 'list-none-s,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
        title 'lists=8' with linespoints lc rgb 'green', \
     "< grep 'list-none-s,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
        title 'lists=16' with linespoints lc rgb 'black'
