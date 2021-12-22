etp: etp.o
	gcc etp.o -o etp
etp.o: etp.c
	gcc -c etp.c -Wall
clean:
	rm -f *.o