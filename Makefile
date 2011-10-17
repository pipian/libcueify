all:
	gcc -Wall -c latin1.c -mno-cygwin
	gcc -Wall -c msjis.c -mno-cygwin
	gcc -Wall -o GenCue GenCue.c cdrom.c toc.c latin1.o msjis.o -mno-cygwin
