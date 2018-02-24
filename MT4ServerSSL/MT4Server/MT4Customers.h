#ifndef MT4CUSTOMERS_H_INCLUDED
#define MT4CUSTOMERS_H_INCLUDED

#include <iostream>
#include <map>
#include <list>
#include <string>

extern "C"
{
#include <pthread.h>
#include <openssl/ssl.h>
}

#include "MT4Customer.h"

class MT4Customers
{
public:
    MT4Customers();

    bool add_customer(int customer_id, const std::string& username, const std::string& password, SSL *ssl, int sock);

    void add_customer_to_operator(int customer_id, int operator_id);
    void remove_customer_from_operator(int customer_id, int operator_id);

    void send_order_to_customers(int operator_id, struct Order order);

    void remove_customer(int customer_id);
    MT4Customer *find_customer_channel(int customer_id);

private:
    pthread_mutex_t customers_lock;
    std::map<int, MT4Customer*>customers;

    pthread_mutex_t relations_lock;
    std::map<int, std::list<int>* > relations;

    std::map<int, pthread_mutex_t*> list_locks;
};

#endif // MT4CUSTOMERS_H_INCLUDED
