all:	main

main:	main.o
	gcc -Wall -ggdb -DLINUX -D_GNU_SOURCE -o main main.o -lvterm 

clean:
	rm -f main *.o valgrind.log.*;
