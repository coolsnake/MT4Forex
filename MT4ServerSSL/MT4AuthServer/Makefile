MT4AuthServer: main.o MT4Listener.o MT4Error.o MT4Authroize.o MT4AuthCluster.o MT4DataBase.o MT4KeepAlive.o MT4Debug.o
	g++ -o MT4AuthServer main.o MT4Listener.o MT4Error.o MT4Authroize.o MT4AuthCluster.o MT4DataBase.o MT4KeepAlive.o MT4Debug.o -lpthread -L/usr/lib/mysql -lmysqlclient

main.o: main.cpp MT4Listener.h MT4AuthCluster.h MT4Error.h
	g++ -c -I/usr/include/mysql main.cpp

MT4Listener.o: MT4Listener.cpp MT4Listener.h
	g++ -c MT4Listener.cpp

MT4Error.o: MT4Error.cpp MT4Error.h
	g++ -c MT4Error.cpp

MT4Authroize.o: MT4Authroize.cpp MT4Authroize.h
	g++ -c -I/usr/include/mysql MT4Authroize.cpp

MT4AuthCluster.o: MT4AuthCluster.cpp MT4AuthCluster.h
	g++ -c -I/usr/include/mysql MT4AuthCluster.cpp

MT4DataBase.o: MT4DataBase.cpp MT4DataBase.h
	g++ -c -I/usr/include/mysql MT4DataBase.cpp

MT4KeepAlive.o: MT4KeepAlive.cpp MT4KeepAlive.h
	g++ -c -I/usr/include/mysql MT4KeepAlive.cpp

MT4Debug.o: MT4Debug.cpp MT4Debug.h
	g++ -c -I/usr/include/mysql MT4Debug.cpp

clean:
	rm *.o MT4AuthServer
