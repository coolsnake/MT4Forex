MT4Server: main.o MT4Listener.o MT4Error.o MT4Authroize.o MT4AuthCluster.o MT4DataBase.o MT4Customers.o MT4Customer.o MT4Operators.o MT4Operator.o MT4Communicator.o MT4SSLAuth.o
	g++ -o MT4Server main.o MT4Listener.o MT4Error.o MT4Authroize.o MT4AuthCluster.o MT4DataBase.o MT4Customers.o MT4Customer.o MT4Operators.o MT4Operator.o MT4Communicator.o MT4SSLAuth.o -lpthread -lssl -L/usr/lib/mysql -lmysqlclient

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

MT4Customers.o: MT4Customers.cpp MT4Customers.h
	g++ -c -I/usr/include/mysql MT4Customers.cpp

MT4Customer.o: MT4Customer.cpp MT4Customer.h
	g++ -c -I/usr/include/mysql MT4Customer.cpp

MT4Operators.o: MT4Operators.cpp MT4Operators.h
	g++ -c -I/usr/include/mysql MT4Operators.cpp

MT4Operator.o: MT4Operator.cpp MT4Operator.h
	g++ -c -I/usr/include/mysql MT4Operator.cpp

MT4Communicator.o: MT4Communicator.cpp MT4Communicator.h
	g++ -c -I/usr/include/mysql MT4Communicator.cpp

MT4SSLAuth.o: MT4SSLAuth.cpp MT4SSLAuth.h
	g++ -c -I/usr/include/mysql MT4SSLAuth.cpp
clean:
	rm *.o MT4Server
