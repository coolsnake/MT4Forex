#ifndef MT4OPERATORS_H_INCLUDED
#define MT4OPERATORS_H_INCLUDED

extern "C"
{
#include <pthread.h>
#include <openssl/ssl.h>
}

#include <map>
#include <list>
#include <string>
#include <iostream>

#include "MT4Operator.h"
#include "MT4Customers.h"

class MT4Operators
{
public:
    MT4Operators(MT4Customers * customers);

    bool add_operator(int operator_id, const std::string& username, const std::string& password, SSL *ssl, int sock);
    std::map<int, MT4Operator*>::iterator operator_exist(int operator_id);

private:
    MT4Customers *Customers;

    pthread_mutex_t operators_lock;
    std::map<int, MT4Operator*> operators;
};

#endif // MT4OPERATORS_H_INCLUDED
