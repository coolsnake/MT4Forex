#include "MT4SSLAuth.h"

MT4SSLAuth::MT4SSLAuth(MT4Authroize *auth, const std::string& public_key, const std::string& private_key)
{
    pthread_mutex_init(&queue_lock, NULL);
    mt4_ssl_auth = auth;
    running = true;
    ctx = SSL_CTX_new(SSLv23_server_method());

    if(ctx == NULL)
    {
        ERR_print_errors_fp(stdout);
    }

    if(SSL_CTX_use_certificate_file(ctx, public_key.c_str(), SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stdout);
    }

    if(SSL_CTX_use_PrivateKey_file(ctx, private_key.c_str(), SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stdout);
    }

    if(!SSL_CTX_check_private_key(ctx))
    {
        ERR_print_errors_fp(stdout);
    }
}

MT4SSLAuth::~MT4SSLAuth()
{
    clear_pending_queue();
    stop_running();
    SSL_CTX_free(ctx);
}


int MT4SSLAuth::purge_pending_queue(fd_set* read_set)
{
    int max_sock = 0;
    int curr_time = time(NULL);
    std::list<struct pend_socket>::iterator it;
    FD_ZERO(read_set);

    pthread_mutex_lock(&queue_lock);
    it = ssl_pending_queue.begin();
    while(it != ssl_pending_queue.end())
    {
        if( curr_time - (*it).create_time > 5)
        {
            std::cout<<"MT4SSLAuth purge socket   "<<(*it).sock<<std::endl;
            SSL_shutdown((*it).ssl);
            SSL_free((*it).ssl);
            close((*it).sock);
            it = ssl_pending_queue.erase(it);
        }
        else
        {
            if((*it).sock > max_sock)
                max_sock = (*it).sock;
            FD_SET((*it).sock, read_set);
            it ++;
        }
    }
    pthread_mutex_unlock(&queue_lock);

    return max_sock;
}

void *MT4SSLAuth::ssl_auth_thread(void *args)
{
    int max_select_sock = 0;
    int alive_sock = 0;
    struct timeval timeout;
    fd_set read_set;
    void *return_value = NULL;
    MT4SSLAuth *mt4_auth = static_cast<MT4SSLAuth*>(args);
    timeout.tv_sec = 0;
    timeout.tv_usec = 10000;
    while(mt4_auth->running)
    {
        pthread_mutex_lock(&mt4_auth->queue_lock);
        if(mt4_auth->ssl_pending_queue.empty())
        {
            pthread_mutex_unlock(&mt4_auth->queue_lock);
            usleep(10000);
            continue;
        }
        pthread_mutex_unlock(&mt4_auth->queue_lock);

        max_select_sock = mt4_auth->purge_pending_queue(&read_set);
        if(max_select_sock > 0)
        {
            max_select_sock ++;

            alive_sock = select(max_select_sock, &read_set, NULL, NULL, &timeout);
            if(alive_sock > 0)
            {
                mt4_auth->handle_auth_safe(&read_set);
            }
        }
    }
    return return_value;
}

void MT4SSLAuth::add(int sock)
{
    int enable = 1;
    struct pend_socket pend;

    setsockopt(sock, SOL_TCP, TCP_NODELAY, &enable, sizeof(enable));

    pend.create_time = time(NULL);
    pend.sock = sock;
    pend.ssl = SSL_new(ctx);
    SSL_set_fd(pend.ssl, pend.sock);

    pthread_mutex_lock(&queue_lock);
    ssl_pending_queue.push_back(pend);
    pthread_mutex_unlock(&queue_lock);
}

int MT4SSLAuth::handle_auth_safe(fd_set *read_set)
{
    std::list<struct pend_socket>::iterator it;
    std::list<struct pend_socket>::iterator jt;
    std::list<struct pend_socket> temp;
    std::list<struct pend_socket> removed;
    int ret;

    temp.clear();
    removed.clear();

    pthread_mutex_lock(&queue_lock);
    it = ssl_pending_queue.begin();
    while( it != ssl_pending_queue.end() )
    {
        temp.push_back((*it));
        it++;
    }
    pthread_mutex_unlock(&queue_lock);

    it = temp.begin();
    while(it != temp.end())
    {
        if( FD_ISSET((*it).sock, read_set) )
        {
            ret = SSL_accept((*it).ssl);
            if(ret == 1)
            {
                std::cout<<"accept one SSL connetion"<<std::endl;
                mt4_ssl_auth->add((*it));
                removed.push_back((*it));
            }
            else if(ret == 0)
            {
                std::cout<<"reject one SSL connection"<<std::endl;
                SSL_shutdown((*it).ssl);
                SSL_free((*it).ssl);
                close((*it).sock);
                removed.push_back((*it));
            }
        }
        it++;
    }

    pthread_mutex_lock(&queue_lock);
    jt = removed.begin();
    while( jt != removed.end() )
    {
        it = ssl_pending_queue.begin();
        while( it != ssl_pending_queue.end() )
        {
            if( (*jt).sock == (*it).sock &&
                (*jt).create_time == (*it).create_time )
            {
                std::cout<<"remove socket out from ssl_pending queue"<<"   "<<(*it).sock<<"   "<<std::endl;
                it = ssl_pending_queue.erase(it);
            }
            else
                it++;
        }
        jt++;
    }
    pthread_mutex_unlock(&queue_lock);

    return 0;
}

void MT4SSLAuth::stop_running()
{
    running = false;
}

void MT4SSLAuth::clear_pending_queue()
{
    std::list<struct pend_socket>::iterator it;

    pthread_mutex_lock(&queue_lock);
    it = ssl_pending_queue.begin();
    while(it != ssl_pending_queue.end())
    {
        SSL_shutdown((*it).ssl);
        SSL_free((*it).ssl);
        close((*it).sock);
        it = ssl_pending_queue.erase(it);
    }
    pthread_mutex_unlock(&queue_lock);
}

