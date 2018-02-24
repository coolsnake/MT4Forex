#ifndef MT4KEEPALIVE_H_INCLUDED
#define MT4KEEPALIVE_H_INCLUDED

extern "C"
{
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <openssl/ssl.h>
}

#include <sys/epoll.h>
#include <pthread.h>
#include <map>
#include <sstream>
#include <iostream>

#include "MT4Error.h"
#include "MT4Datatype.h"
#include "MT4Debug.h"

#define MAX_POLLING_QUEUE 1024
#define MAX_EVENTS 128


class MT4KeepAlive
{
public:
    MT4KeepAlive()
    {
        epoll_fd = epoll_create(MAX_POLLING_QUEUE);
        if(epoll_fd == -1)
        {
            throw(MT4KeepAliveError("Initialize Epoll Socket failed"));
            exit(-1);
        }
        keep_alive_users.clear();
        ssl_com_channel.clear();
        pthread_mutex_init(&users_lock, NULL);
    };

    ~MT4KeepAlive()
    {
        std::map<std::string, int>::iterator it;
        std::map<int, SSL*>::iterator st;
        for(st = ssl_com_channel.begin(); st != ssl_com_channel.end(); st++)
        {
            SSL_shutdown((*st).second);
            SSL_free((*st).second);
        }
        for(it = keep_alive_users.begin(); it != keep_alive_users.end(); it++)
            close((*it).second);
        ssl_com_channel.clear();
        keep_alive_users.clear();
    };

    void setnonblocking(int sockfd);

    void add_alive_users(const std::string&username, int sockfd, SSL *ssl, long EAID);
    bool handle_keep_alive(int sockfd);

    static void *keep_alive_thread(void *args);

private:
    int epoll_fd;
    pthread_mutex_t users_lock;
    std::map<std::string, int> keep_alive_users;
    std::map<int, SSL*> ssl_com_channel;
};

#endif MT4KEEPALIVE_H_INCLUDED
