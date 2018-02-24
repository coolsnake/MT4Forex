#ifndef MT4LISTENER_H_INCLUDED
#define MT4LISTENER_H_INCLUDED

#include <string>
#include "MT4Debug.h"

extern "C"
{
#include <netinet/in.h>
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

#define SERVER_PORT 32348

class MT4Listener {
public:
    MT4Listener(const std::string& host);
    ~MT4Listener();

    void start_listen();
    int accept_conn();

private:
    int sock;
    bool listening;
    std::string host_name;
};

#endif // MT4LISTENER_H_INCLUDED
