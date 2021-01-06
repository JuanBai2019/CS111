#NAME:Juan Bai
#EMAIL:Daisybai66@yahoo.com
#ID:105364754



default: lab1a

lab1a: lab1a.c
	/usr/local/cs/bin/gcc lab1a.c -o lab1a -g -Wall -Wextra

clean:
	rm -rf lab1a lab1a-105364754.tar.gz

dist:lab1a.c Makefile README
	tar -czvf lab1a-105364754.tar.gz lab1a.c Makefile README
