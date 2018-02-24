#ifndef MT4COMMUNICATOR_H_INCLUDED
#define MT4COMMUNICATOR_H_INCLUDED
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
#include <stdlib.h>
}

#include "MT4Datatype.h"
#include "MT4Command.h"
#include <string>

class MT4Communicator {

public:
    MT4Communicator(const std::string& hostname, int port, const std::string& name, const std::string& password);
    ~MT4Communicator();

    void command_parse(const struct MT4Command &mt4_cmd);

    static void *communicator_thread(void* arg);

private:
    std::string auth_name;
    std::string auth_password;

    bool listening;
    int sock;

};

#endif
