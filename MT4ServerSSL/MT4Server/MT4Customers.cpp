#include "MT4Customers.h"

MT4Customers::MT4Customers()
{
    pthread_mutex_init(&customers_lock, NULL);
    pthread_mutex_init(&relations_lock, NULL);
    customers.clear();
    relations.clear();
}

void MT4Customers::add_customer_to_operator(int customer_id, int operator_id)
{
    std::map<int, std::list<int>* >::iterator it;
    std::map<int, pthread_mutex_t*>::iterator pt;


    std::cout<<"add customer   "<<customer_id<<"   to operator   "<<operator_id<<std::endl;

    pthread_mutex_lock(&relations_lock);
    it = relations.find( operator_id );
    pt = list_locks.find( operator_id );
    pthread_mutex_unlock(&relations_lock);

    if( it != relations.end() )
    {
        pthread_mutex_lock((*pt).second);
        (*it).second->push_back(customer_id);
        (*it).second->sort();
        (*it).second->unique();
        pthread_mutex_unlock((*pt).second);
    }
    else
    {
        pthread_mutex_lock(&relations_lock);

        pthread_mutex_t *lock;
        lock = new pthread_mutex_t;
        pthread_mutex_init(lock, NULL);
        list_locks.insert( std::pair<int, pthread_mutex_t*>(operator_id, lock) );
        std::list<int>* customer_queue = new std::list<int>;
        relations.insert( std::pair<int, std::list<int>* >(operator_id, customer_queue) );
        relations[operator_id]->push_back(customer_id);

        pthread_mutex_unlock(&relations_lock);

        std::cout<<"The new customer queue size for operator  "<<operator_id<<"    "<<relations[operator_id]->size()<<std::endl;
    }
}

void MT4Customers::remove_customer_from_operator(int customer_id, int operator_id)
{
    std::map<int, std::list<int>* >::iterator it;
    std::map<int, pthread_mutex_t*>::iterator pt;

    pthread_mutex_lock(&relations_lock);
    it = relations.find( operator_id );
    pt = list_locks.find( operator_id );
    pthread_mutex_unlock(&relations_lock);

    if( it != relations.end() )
    {
        pthread_mutex_lock((*pt).second);
        (*it).second->remove(customer_id);
        pthread_mutex_unlock((*pt).second);
    }
}

bool MT4Customers::add_customer(int customer_id, const std::string& username, const std::string& password, SSL *ssl, int sock)
{
    std::map<int, MT4Customer*>::iterator it;

    pthread_mutex_lock( &customers_lock );

    it = customers.find(customer_id);
    if( it != customers.end() )
    {
        std::cout<<"MT4Customers add orginal existed customer   "<<username<<std::endl;
        (*it).second->update( ssl, sock );
    }
    else
    {
        MT4Customer *customer = new MT4Customer(customer_id, username, password, ssl, sock);
        std::cout<<"MT4Customers add one customer    "<<username<<std::endl;
        customers.insert( std::pair<int, MT4Customer*>(customer_id, customer) );
    }

    pthread_mutex_unlock( &customers_lock );
}

void MT4Customers::send_order_to_customers(int operator_id, struct Order order)
{
    std::map<int, std::list<int>* >::iterator it;
    std::map<int, pthread_mutex_t*>::iterator pt;
    std::list<int>::iterator nt;
    std::list<int> snap;
    MT4Customer *cu;
    bool send_status;

    std::cout<<"Starting Send out Order To Customers  OpID   "<<operator_id<<std::endl;
    snap.clear();

    pthread_mutex_lock(&relations_lock);
    it = relations.find( operator_id );
    pt = list_locks.find( operator_id );
    pthread_mutex_unlock(&relations_lock);

    std::cout<<"before find relations   "<<relations.size()<<std::endl;

    if( it != relations.end() )
    {
        std::cout<<"There is Customers related to Operator"<<std::endl;

        pthread_mutex_lock((*pt).second);
        nt = (*it).second->begin();
        while(nt != (*it).second->end())
        {
            snap.push_back((*nt));
            nt++;
        }
        pthread_mutex_unlock((*pt).second);

        if(snap.size() == 0)
           std::cout<<"BIG ERROR OF THE CUSTOMER QUEUE"<<std::endl;

        nt = snap.begin();
        while( nt != snap.end() )
        {
            cu = find_customer_channel((*nt));
            if(cu != NULL) {
                send_status = cu->send_order(order);
                if(!send_status)
                {
                    std::cout<<"Can not Send out Order successfully  remove it"<<std::endl;
                    remove_customer((*nt));
                }
            }
            nt++;
        }
    }
}

MT4Customer *MT4Customers::find_customer_channel(int customer_id)
{
    std::map<int, MT4Customer*>::iterator it;
    MT4Customer *cu;

    std::cout<<"Staring to find out the Customers channel     "<<customer_id<<std::endl;
    pthread_mutex_lock( &customers_lock );
    it = customers.find(customer_id);
    if(it != customers.end())
    {
        cu = (*it).second;
    }
    else
    {
        cu = NULL;
    }
    pthread_mutex_unlock( &customers_lock );
    return(cu);
}

void MT4Customers::remove_customer(int customer_id)
{
    std::map<int, MT4Customer*>::iterator it;

    pthread_mutex_lock( &customers_lock );
    it = customers.find(customer_id);
    if( it != customers.end())
    {
        delete((*it).second);
        customers.erase(it);
    }
    pthread_mutex_unlock( &customers_lock );
}
