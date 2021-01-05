#NAME:Juan Bai
#EMAIL:Daisybai66@yahoo.com
#ID:105364754

default: lab0

lab0: lab0.c
	/usr/local/cs/bin/gcc lab0.c -o lab0 -g -Wall -Wextra

check: lab0 input_file
#test input and output
	./lab0 --input=input_file  --output=output_file

#compare the differences
	cat input_file 
	cat output_file 

#test input option
	./lab0 --input=input_file
#test output option
	./lab0 --output=output_file
	cat output_file

#test stdin/stdout
	./lab0

clean:
	rm -rf lab0-105364754.tar.gz lab0 output_file

dist: lab0.c Makefile  README input_file backtrace.png breakpoint.png 
	tar -czvf lab0-105364754.tar.gz lab0.c Makefile README input_file backtrace.png breakpoint.png
