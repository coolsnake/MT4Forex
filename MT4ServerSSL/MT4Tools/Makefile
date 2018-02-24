MT4Tools:mt4_client.o
	gcc -o MT4Tools mt4_client.o -lpthread

mt4_client.o: mt4_client.c mt4_client.h mt4_datatype.h
	gcc -c mt4_client.c

clean:
	rm *.o MT4Tools
