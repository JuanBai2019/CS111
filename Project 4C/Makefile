# NAME: Juan Bai
# EMAIL: daisybai66@yahoo.com
# ID: 105364754


default: 
	gcc -g -Wall -Wextra -lmraa -lm -o lab4c_tcp lab4c_tcp.c
	gcc -g -Wall -Wextra -lmraa -lm -lssl -lcrypto -o lab4c_tls lab4c_tls.c

clean:
	rm -f lab4c-105364754.tar.gz lab4c_tcp lab4c_tls

dist:
	tar -czvf lab4c-105364754.tar.gz README Makefile lab4c_tls.c lab4c_tcp.c

