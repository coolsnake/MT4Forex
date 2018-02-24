#include "MT4Customer.h"

MT4Customer::MT4Customer(int customer_id, const std::string& user, const std::string& pass, SSL *cu_ssl, int cu_sock)
{
    ID = customer_id;
    username = user;
    password = pass;
    sock = cu_sock;
    ssl = cu_ssl;
    reference_count = 0;
}

MT4Customer::~MT4Customer()
{
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sock);
}

bool MT4Customer::send_order(struct Order order)
{
    int length;
    length = SSL_write(ssl, &order, sizeof(struct Order));
    if(length == sizeof(struct Order))
        return true;
    else
        return false;
}

void MT4Customer::update(SSL *cu_ssl, int s)
{
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sock);
    sock = s;
    ssl = cu_ssl;
}
