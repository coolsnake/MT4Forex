#ifndef MT4SSLAUTH_H_INCLUDED
#define MT4SSLAUTH_H_INCLUDED
#include <iostream>
#include <list>

extern "C"
{
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/select.h>
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
#include <openssl/err.h>
}

#include "MT4Datatype.h"
#include "MT4DataBase.h"
#include "MT4Authroize.h"

class MT4SSLAuth
{
public:
    MT4SSLAuth(MT4Authroize *auth, const std::string &public_key, const std::string &private_key);
    ~MT4SSLAuth();
    void add(int sock);

    static void *ssl_auth_thread(void *args);

private:
   void clear_pending_queue();
    int get_cur_pos();
    int purge_pending_queue(fd_set *read_set);
    int handle_auth_safe(fd_set *read_set);
    void stop_running();

private:
    bool running;
    pthread_mutex_t queue_lock;
    std::list<struct pend_socket> ssl_pending_queue;
    SSL_CTX *ctx;

    MT4Authroize *mt4_ssl_auth;
};
#endif
