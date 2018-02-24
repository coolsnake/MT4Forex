#include "MT4KeepAlive.h"

void MT4KeepAlive::setnonblocking(int sockfd)
{
    int fileflag;
    fileflag = fcntl( sockfd, F_GETFL );
    fileflag |= O_NONBLOCK;
    fcntl( sockfd, F_SETFL, fileflag );
}

void MT4KeepAlive::add_alive_users(const std::string& username, int sockfd, SSL *ssl, long EAID)
{
    std::map< std::string, int>::iterator it;
    std::map< int, SSL*>::iterator st;
    std::stringstream stream;
    stream<<EAID;
    std::string id;
    stream>>id;
    std::string key = username + id;
    struct epoll_event ev;

    pthread_mutex_lock(&users_lock);
    it = keep_alive_users.find( key );
    if( it != keep_alive_users.end() )
    {
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = (*it).second;
        epoll_ctl( epoll_fd, EPOLL_CTL_DEL, (*it).second, &ev );
        st = ssl_com_channel.find((*it).second);
        if(st != ssl_com_channel.end())
        {
            SSL_shutdown((*st).second);
            SSL_free((*st).second);
            (*st).second = ssl;
        }
        close((*it).second);

        setnonblocking(sockfd);
        (*it).second = sockfd;
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = sockfd;
        epoll_ctl( epoll_fd, EPOLL_CTL_ADD, sockfd, &ev );
    }
    else
    {
        keep_alive_users.insert( std::pair<std::string, int>( key, sockfd ) );
        ssl_com_channel.insert( std::pair<int, SSL*>(sockfd, ssl));
        setnonblocking( sockfd );
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = sockfd;
        epoll_ctl( epoll_fd, EPOLL_CTL_ADD, sockfd, &ev );
    }
    pthread_mutex_unlock(&users_lock);
};

void *MT4KeepAlive::keep_alive_thread(void *args)
{
    MT4KeepAlive *mt4_keep = static_cast<MT4KeepAlive*>(args);
    std::map<int, SSL*>::iterator st;
    struct epoll_event events[MAX_EVENTS];
    struct epoll_event ev;
    int nfds;
    bool status;

    for(;;)
    {
        nfds = epoll_wait( mt4_keep->epoll_fd, events, MAX_EVENTS, -1 );// Can be change to 0 to test the parallel operations on epoll
        for(int i=0; i<nfds; i++)
        {
            status = mt4_keep->handle_keep_alive(events[i].data.fd);
            if (!status)
            {
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = events[i].data.fd;
                epoll_ctl( mt4_keep->epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, &ev );
                st = mt4_keep->ssl_com_channel.find(events[i].data.fd);
                if(st!= mt4_keep->ssl_com_channel.end())
                {
                    SSL_shutdown((*st).second);
                    SSL_free((*st).second);
                }
                close( events[i].data.fd );
                DEBUG("Socket exception, close it");
            }
        }
    }
};

/*
 MT4KeepAlive::handle_keep_alive
 will handle the normal connection, and EAGAIN error and also normal disconnection.
*/
bool MT4KeepAlive::handle_keep_alive(int sockfd)
{
    struct auth_keep_alive auth_buff;
    std::map< int, SSL*>::iterator st;
    SSL *ssl;
    int recv_length = 0;
    int send_length = 0;
    st = ssl_com_channel.find(sockfd);
    if(st != ssl_com_channel.end())
    {
        ssl = (*st).second;
        recv_length = SSL_read(ssl, &auth_buff.auth, sizeof(struct auth_keep_alive));
        if (recv_length > 0)
        {
            send_length = SSL_write(ssl, &auth_buff, sizeof(struct auth_keep_alive));
            if (send_length > 0)
                return true;
            else if(send_length < 0)
                return true;
            else
                return false;
        } else if(recv_length < 0)
            return true;
        else
            return false;
    }
};
