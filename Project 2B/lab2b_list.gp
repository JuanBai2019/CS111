#! /usr/local/cs/bin/gnuplot

#NAME: Juan Bai
#EMAIL: daisybai66@yahoo.com
#ID: 105364754

# purpose:
#  generate data reduction graphs for the multi-threaded list project
#
# input: lab2_list.csv
# 1. test name
# 2. # threads
# 3. # iterations per thread
# 4. # lists
# 5. # operations performed (threads x iterations x (ins + lookup + delete))
# 6. run time (ns)
# 7. run time per operation (ns)
# output:
#lab2b_1.png ... throughput vs. number of threads for mutex and spin-lock synchronized list operations.
#lab2b_2.png ... mean time per mutex wait and mean time per operation for mutex-synchronized list operations.
#lab2b_3.png ... successful iterations vs. threads for each synchronization method.
#lab2b_4.png ... throughput vs. number of threads for mutex synchronized partitioned lists.
#lab2b_5.png ... throughput vs. number of threads for spin-lock-synchronized partitioned lists.
set terminal png
set datafile separator ","

set title "Throughput vs Number of Threads"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Throughput(Opes/sec)"
set logscale y 10
set output 'lab2b_1.png'
plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title 'w/mutex' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title 'w/spin-lock' with linespoints lc rgb 'green'

set title "List-2: Mean Time/Mutex wait and mean time/operation "
set xlabel "Threads"
set logscale x 2
unset xrange
set xrange [0.75:]
set ylabel "Time"
set logscale y 10
set output 'lab2b_2.png'
set key left top
plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($8) \
	title 'mutex average wait-for-lock time' with linespoints lc rgb 'red', \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($7) \
	title 'mutex average time per operation' with linespoints lc rgb 'green'


set title "List-3: Successful Iterations vs Threads"
set xlabel "Threads"
set logscale x 2
unset xrange
set xrange [0.75:]
set ylabel "Iterations succeeded"
set logscale y 10
set output 'lab2b_3.png'
set key left top
plot \
     "< grep 'list-id-none,[0-9]*,[0-9]*,4,' lab2b_list.csv" using ($2):($3) \
	title 'without synchronization' with points lc rgb 'red', \
     "< grep 'list-id-m,[0-9]*,[0-9]*,4,' lab2b_list.csv" using ($2):($3) \
	title 'mutex protected' with points lc rgb 'green', \
     "< grep 'list-id-s,[0-9]*,[0-9]*,4,' lab2b_list.csv" using ($2):($3) \
	title 'spin-lock protected' with points lc rgb 'orange'


set title "List-4: Performance of partitioned lists with mutex"
set xlabel "Threads"
set logscale x 2
unset xrange
set xrange [0.75:]
set ylabel "Throughput"
set logscale y 10
set output 'lab2b_4.png'
set key left top
plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
        title '1 sublist' with linespoints lc rgb 'red', \
	"< grep 'list-none-m,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
        title '4 sublists' with linespoints lc rgb 'green', \
	"< grep 'list-none-m,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
        title '8 sublists' with linespoints lc rgb 'orange', \
	"< grep 'list-none-m,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
        title '16 sublists' with linespoints lc rgb 'blue'


set title "List-5: Performance of partitioned lists with spin-lock"
set xlabel "Threads"
set logscale x 2
unset xrange
set xrange [0.75:]
set ylabel "Throughput"
set logscale y 10
set output 'lab2b_5.png'
set key left top
plot \
     "< grep 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
        title '1 sublist' with linespoints lc rgb 'red', \
	"< grep 'list-none-s,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
        title '4 sublists' with linespoints lc rgb 'green', \
	"< grep 'list-none-s,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
        title '8 sublists' with linespoints lc rgb 'orange', \
	"< grep 'list-none-s,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
        title '16 sublists' with linespoints lc rgb 'blue'