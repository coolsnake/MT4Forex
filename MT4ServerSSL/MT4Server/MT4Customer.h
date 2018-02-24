#ifndef MT4CUSTOMER_H_INCLUDED
#define MT4CUSTOMER_H_INCLUDED

extern "C"
{
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


#include <string>
#include <iostream>

#include "MT4Datatype.h"

class MT4Customer
{
public:
    MT4Customer(int customer_id, const std::string& user, const std::string& pass, SSL *cu_ssl, int cu_sock);
    ~MT4Customer();

    void add_ref();
    void dec_ref();

    int get_sock(){
        return sock;
    };

    void update(SSL *cu_ssl, int s);

    bool send_order(struct Order);

private:
    int reference_count;
    int ID;
    std::string username;
    std::string password;
    int sock;
    SSL *ssl;
};

#endif // MT4CUSTOMER_H_INCLUDED
