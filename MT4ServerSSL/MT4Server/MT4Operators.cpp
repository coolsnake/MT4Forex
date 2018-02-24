#include "MT4Operators.h"

MT4Operators::MT4Operators(MT4Customers *customers)
{
    pthread_mutex_init(&operators_lock, NULL);
    Customers = customers;
}

bool MT4Operators::add_operator(int operator_id, const std::string& username, const std::string& password, SSL *ssl, int sock)
{
    std::map<int,MT4Operator*>::iterator it;
    std::pair<std::map<int,MT4Operator*>::iterator,bool> ret;

    pthread_mutex_lock(&operators_lock);
    it = operator_exist( operator_id );

    if( it == operators.end() )
    {
        std::cout<<"MT4Operators add one operator to map    "<<username<<std::endl;
        MT4Operator *op = new MT4Operator(operator_id, username, password, ssl, sock);
        ret = operators.insert( std::pair<int, MT4Operator*>(operator_id, op) );
        if(ret.second)
            it = ret.first;
        else
            it = operators.end();
    }
    else
    {
        std::cout<<"MT4Operators add original existed operator   "<<username<<std::endl;
        delete((*it).second);
        operators.erase(it);
        MT4Operator *op = new MT4Operator(operator_id, username, password, ssl, sock);
        ret = operators.insert( std::pair<int, MT4Operator*>(operator_id, op) );
        if(ret.second)
            it = ret.first;
        else
            it = operators.end();
    }

    if(it != operators.end())
    {
        (*it).second->set_customers(Customers);
        pthread_t pid;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setstacksize(&attr, 0x4000);

        std::cout<<"starting new receiving thread"<<std::endl;
        if (pthread_create(&pid, &attr, &MT4Operator::operator_thread, (void*)((*it).second)) == 0) {
            (*it).second->set_pthread_id(pid);
        } else {
            std::cout<<"new thread creating failed desctruct it"<<std::endl;
            delete((*it).second);
            operators.erase(it);
        }
    }

    pthread_mutex_unlock(&operators_lock);

}

std::map<int, MT4Operator*>::iterator MT4Operators::operator_exist(int operator_id)
{
    std::map<int,MT4Operator*>::iterator it;
    it = operators.find(operator_id);
    return it;
}
