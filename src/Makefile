all:
	gcc -Wall -c latin1.c -mno-cygwin
	gcc -Wall -c msjis.c -mno-cygwin
	gcc -Wall -c cdrom.c -mno-cygwin
	gcc -Wall -c toc.c -mno-cygwin
	ar rcs libcueify.a latin1.o msjis.o cdrom.o toc.o
