#ifndef MT4OPERATOR_H_INCLUDED
#define MT4OPERATOR_H_INCLUDED

#include <list>
#include <string>
#include <iostream>

extern "C"
{
#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <openssl/ssl.h>
}

#include "MT4Datatype.h"
#include "MT4Customers.h"

class MT4Operator
{
public:
    MT4Operator(int operator_id, const std::string& user, const std::string& pass, SSL *op_ssl, int op_sock);
    ~MT4Operator();

    void set_customers(MT4Customers *customers);
    void set_pthread_id(pthread_t id) {
        pid = id;
    };

    static void *operator_thread(void *args);

private:
    int sock;
    SSL *ssl;
    int ID;
    std::string username;
    std::string password;
    bool running;
    pthread_t pid;
    MT4Customers *Customers;
};


#endif // MT4OPERATOR_H_INCLUDED
