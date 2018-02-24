mt4_follow: mt4_follow.o
	gcc -o mt4_follow mt4_follow.o -lpthread

mt4_follow.o: mt4_follow.c mt4_follow.h mt4_datatype.h
	gcc -c mt4_follow.c

clean:
	rm *.o mt4_follow
