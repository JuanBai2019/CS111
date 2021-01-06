#NAME:Juan Bai
#EMAIL:Daisybai66@yahoo.com
#ID:105364754



default: lab1b-client lab1b-server

lab1b-client: lab1b-client.c
	/usr/local/cs/bin/gcc lab1b-client.c -o lab1b-client -g -lz -Wall -Wextra

lab1b-server: lab1b-server.c
	/usr/local/cs/bin/gcc lab1b-server.c -o lab1b-server -g -lz -Wall -Wextra

clean:
	rm -rf lab1b-client lab1b-server lab1b-105364754.tar.gz

dist:lab1b-client.c lab1b-server.c Makefile README
	tar -czvf lab1b-105364754.tar.gz lab1b-client.c lab1b-server.c  Makefile README

