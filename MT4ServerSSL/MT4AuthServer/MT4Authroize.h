#ifndef MT4AUTHROIZE_H_INCLUDED
#define MT4AUTHROIZE_H_INCLUDED
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
}

#include "MT4Datatype.h"
#include "MT4DataBase.h"
#include "MT4KeepAlive.h"

#define OPERATOR_MASK  0x08
#define CUSTOMER_MASK  0x01

class MT4Authroize{
public:
    MT4Authroize(const std::string& host, const std::string& username, const std::string& password, const std::string& database, MT4KeepAlive *KA);
    ~MT4Authroize();

    static void *auth_check_thread(void *args);

    void add(struct pend_socket sock);

private:
    void clear_pending_queue();
    int get_cur_pos();
    int purge_pending_queue(fd_set *read_set);
    int handle_auth_safe(fd_set *read_set);
    void stop_running();
private:
    bool running;
    pthread_mutex_t queue_lock;
    std::list<struct pend_socket> pending_queue;

    pthread_t MT4_db_pid;
    MT4Database *MT4_db;

    int auth_pass;
    int auth_fail;

    MT4KeepAlive *MT4_KA;
};

#endif // MT4AUTHROIZE_H_INCLUDED
