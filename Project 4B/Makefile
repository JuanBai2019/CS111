# NAME: Juan Bai
# EMAIL: daisybai66@yahoo.com
# ID: 105364754


default: lab4b 

lab4b: lab4b.c compile.sh
	chmod 755 compile.sh
	./compile.sh
compile:
	uname -a | grep "armv7l";\
	if [ $$?  -eq 0 ];\
	then \
	     /usr/local/cs/bin/gcc lab4b.c -o lab4b -g -lm -lmraa -Wall -Wextra; \
	else \
	     /usr/local/cs/bin/gcc lab4b.c -o lab4b -g -DDUMMY -Wall -Wextra -lm; \
	fi;


check:
	{ echo "PERIOD=2"; sleep 2; echo "STOP"; echo "SCALE=C"; sleep 2; echo "START"; sleep 2; echo "OFF"; } | ./lab4b --log=logfile.txt
	rm -f logfile.txt
clean: 
	rm -f lab4b lab4b-105364754.tar.gz 

dist: 
	tar -czvf lab4b-105364754.tar.gz lab4b.c Makefile README compile.sh
