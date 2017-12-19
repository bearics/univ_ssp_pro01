make: 
	gcc -o a.out -g -ggdb *.c -lpthread

clean:
	rm -f *.o