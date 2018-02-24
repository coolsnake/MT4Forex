#include "MT4Operator.h"

MT4Operator::MT4Operator(int operator_id, const std::string& user, const std::string& pass, SSL *op_ssl, int op_sock)
{
    ID = operator_id;
    username = user;
    password = pass;
    sock = op_sock;
    ssl = op_ssl;
    running = true;
    pid = 0;
}

MT4Operator::~MT4Operator()
{
    if(ssl != NULL)
    {
        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(sock);
    }
    running = false;
    if(pid != 0)
        pthread_join(pid, NULL);
    std::cout<<"Complete the destroy routine"<<std::endl;
}

void MT4Operator::set_customers(MT4Customers *customers)
{
    Customers = customers;
}

void * MT4Operator::operator_thread(void *args)
{
    MT4Operator *op = static_cast<MT4Operator*>(args);
    void *return_value = NULL;
    int length;
    int send_length;
    int cli_sock;
    int sel_res;
    fd_set readfds;
    struct Order order;
    struct timeval tv;

    std::cout<<"Operating Thread   "<<op->username<<std::endl;
    cli_sock = op->sock;
    while(op->running)
    {
        tv.tv_sec = 0;
        tv.tv_usec = 500000;
        FD_ZERO(&readfds);
        FD_SET(cli_sock, &readfds);
        sel_res = select(cli_sock + 1, &readfds, NULL, NULL, &tv);
        if(sel_res > 0)
        {
            length = SSL_read(op->ssl, &order, sizeof(struct Order));
            if(length == sizeof(struct Order))
            {
                std::cout<<"MT4Operator  Get one order from operator   "<<op->username<<std::endl;
                op->Customers->send_order_to_customers( op->ID, order);
            }
            else if (length < 0)
            {
                usleep(10000);
            }
            else
            {
                std::cout<<"shutdown ssl connection from operator"<<std::endl;
                if(op->ssl != NULL)
                {
                    SSL_shutdown(op->ssl);
                    SSL_free(op->ssl);
                    close(op->sock);
                    op->ssl = NULL;
                }
                std::cout<<"disconnect from operator"<<std::endl;
                return return_value;
            }
        }
    }
    std::cout<<"Exit From Thread   "<<op->username<<std::endl;

    return return_value;
}
