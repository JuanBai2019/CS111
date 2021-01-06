#NAME:Juan Bai
#EMAIL:Daisybai66@yahoo.com
#ID:105364754



default: 
	/usr/local/cs/bin/gcc lab2_list.c SortedList.c -g -o lab2_list -lpthread -Wall -Wextra


graphs: tests
	gnuplot ./lab2b_list.gp


clean:
	rm -rf lab2_list lab2b-105364754.tar.gz

dist: graphs
	tar -czvf lab2b-105364754.tar.gz lab2_list.c SortedList.h SortedList.c Makefile lab2b_list.csv README lab2b_list.gp *.png profile.out




tests:default
	rm -rf lab2b_list.csv

	#lab2b_1.png & lab2b_2.png
	./lab2_list --threads=1  --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=s >> lab2b_list.csv

	#lab2b_3.png
	./lab2_list --threads=1 --iterations=1  --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=2  --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=4  --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=8  --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=16 --lists=4 --yield=id >> lab2b_list.csv

	-./lab2_list --threads=4 --iterations=1  --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=2  --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=4  --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=8  --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=16 --lists=4 --yield=id >> lab2b_list.csv

	-./lab2_list --threads=8 --iterations=1  --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=2  --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=4  --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=8  --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=16 --lists=4 --yield=id >> lab2b_list.csv

	-./lab2_list --threads=12 --iterations=1  --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=2  --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=4  --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=8  --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=16 --lists=4 --yield=id >> lab2b_list.csv

	-./lab2_list --threads=16 --iterations=1  --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=2  --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=4  --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=8  --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=16 --lists=4 --yield=id >> lab2b_list.csv

	./lab2_list --threads=1 --iterations=10 --lists=4 --yield=id --sync=s >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=20 --lists=4 --yield=id --sync=s >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=40 --lists=4 --yield=id --sync=s >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=80 --lists=4 --yield=id --sync=s >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=10 --lists=4 --yield=id --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=20 --lists=4 --yield=id --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=40 --lists=4 --yield=id --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=80 --lists=4 --yield=id --sync=m >> lab2b_list.csv

	-./lab2_list --threads=4 --iterations=10 --lists=4 --yield=id --sync=s >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=20 --lists=4 --yield=id --sync=s >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=40 --lists=4 --yield=id --sync=s >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=80 --lists=4 --yield=id --sync=s >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=10 --lists=4 --yield=id --sync=m >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=20 --lists=4 --yield=id --sync=m >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=40 --lists=4 --yield=id --sync=m >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=80 --lists=4 --yield=id --sync=m >> lab2b_list.csv

	-./lab2_list --threads=8 --iterations=10 --lists=4 --yield=id --sync=s >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=20 --lists=4 --yield=id --sync=s >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=40 --lists=4 --yield=id --sync=s >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=80 --lists=4 --yield=id --sync=s >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=10 --lists=4 --yield=id --sync=m >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=20 --lists=4 --yield=id --sync=m >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=40 --lists=4 --yield=id --sync=m >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=80 --lists=4 --yield=id --sync=m >> lab2b_list.csv

	-./lab2_list --threads=12 --iterations=10 --lists=4 --yield=id --sync=s >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=20 --lists=4 --yield=id --sync=s >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=40 --lists=4 --yield=id --sync=s >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=80 --lists=4 --yield=id --sync=s >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=10 --lists=4 --yield=id --sync=m >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=20 --lists=4 --yield=id --sync=m >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=40 --lists=4 --yield=id --sync=m >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=80 --lists=4 --yield=id --sync=m >> lab2b_list.csv

	-./lab2_list --threads=16 --iterations=10 --lists=4 --yield=id --sync=s >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=20 --lists=4 --yield=id --sync=s >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=40 --lists=4 --yield=id --sync=s >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=80 --lists=4 --yield=id --sync=s >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=10 --lists=4 --yield=id --sync=m >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=20 --lists=4 --yield=id --sync=m >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=40 --lists=4 --yield=id --sync=m >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=80 --lists=4 --yield=id --sync=m >> lab2b_list.csv

	#lab2b_4.png
	./lab2_list --threads=1  --iterations=1000 --lists=1  --sync=m >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --lists=4  --sync=m >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --lists=8  --sync=m >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --lists=16 --sync=m >> lab2b_list.csv

	-./lab2_list --threads=2  --iterations=1000 --lists=1  --sync=m >> lab2b_list.csv
	-./lab2_list --threads=2  --iterations=1000 --lists=4  --sync=m >> lab2b_list.csv
	-./lab2_list --threads=2  --iterations=1000 --lists=8  --sync=m >> lab2b_list.csv
	-./lab2_list --threads=2  --iterations=1000 --lists=16 --sync=m >> lab2b_list.csv

	-./lab2_list --threads=4  --iterations=1000 --lists=1  --sync=m >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=1000 --lists=4  --sync=m >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=1000 --lists=8  --sync=m >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=1000 --lists=16 --sync=m >> lab2b_list.csv

	-./lab2_list --threads=8  --iterations=1000 --lists=1  --sync=m >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=1000 --lists=4  --sync=m >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=1000 --lists=8  --sync=m >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=1000 --lists=16 --sync=m >> lab2b_list.csv

	-./lab2_list --threads=12 --iterations=1000 --lists=1  --sync=m >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=1000 --lists=4  --sync=m >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=1000 --lists=8  --sync=m >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=1000 --lists=16 --sync=m >> lab2b_list.csv

	#lab2b_5.png
	./lab2_list --threads=1  --iterations=1000 --lists=1  --sync=s >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --lists=4  --sync=s >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --lists=8  --sync=s >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --lists=16 --sync=s >> lab2b_list.csv

	-./lab2_list --threads=2  --iterations=1000 --lists=1  --sync=s >> lab2b_list.csv
	-./lab2_list --threads=2  --iterations=1000 --lists=4  --sync=s >> lab2b_list.csv
	-./lab2_list --threads=2  --iterations=1000 --lists=8  --sync=s >> lab2b_list.csv
	-./lab2_list --threads=2  --iterations=1000 --lists=16 --sync=s >> lab2b_list.csv

	-./lab2_list --threads=4  --iterations=1000 --lists=1  --sync=s >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=1000 --lists=4  --sync=s >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=1000 --lists=8  --sync=s >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=1000 --lists=16 --sync=s >> lab2b_list.csv

	-./lab2_list --threads=8  --iterations=1000 --lists=1  --sync=s >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=1000 --lists=4  --sync=s >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=1000 --lists=8  --sync=s >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=1000 --lists=16 --sync=s >> lab2b_list.csv

	-./lab2_list --threads=12 --iterations=1000 --lists=1  --sync=s >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=1000 --lists=4  --sync=s >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=1000 --lists=8  --sync=s >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=1000 --lists=16 --sync=s >> lab2b_list.csv

profile: default
	rm -rf profile.out
	LD_PRELOAD=/usr/lib64/libprofiler.so CPUPROFILE=./raw.gperf ./lab2_list --threads=12 --iterations=1000 --sync=s
	# LD_PRELOAD: link the library, overload existing function in my_prog
	# CPUPROFILE: gperf specific environment variable, tells gperf where to store the raw         
	# profiling data. (perform sampling on ./lab2_list output to ./raw.gperf)
	pprof   --text ./lab2_list ./raw.gperf >profile.out
	#analyze raw sampling file, show sampling results in functions (i.e. how often does sampling fall into individual functions) step 4,5 generates readable output from raw output

	pprof   --list=thread_worker ./lab2_list ./raw.gperf >>profile.out 
	# show sampling results in function thread_worker (i.e. how often does sampling fall into     each lines of code in thread_worker)
	rm -f ./raw.gperf
