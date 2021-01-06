#!/bin/bash
uname -a | grep "armv7l"
if [ $?  -eq 0 ]
then
     /usr/local/cs/bin/gcc lab4b.c -o lab4b -g -lm -lmraa -Wall -Wextra 
else
     /usr/local/cs/bin/gcc lab4b.c -o lab4b -g -DDUMMY -Wall -Wextra -lm
fi
