#include "MT4Listener.h"
#include "MT4Error.h"

MT4Listener::MT4Listener(const std::string& host)
{
    host_name = host;
    sock = socket(PF_INET, SOCK_STREAM, 0);
    listening = false;
}

MT4Listener::~MT4Listener()
{
    close(sock);
}

void MT4Listener::start_listen()
{
    struct sockaddr_in server_addr;
    struct hostent *hp;
    const char *ip_addr;
    char server_ip[20];
    int reuse_addr = 1;

    if(listening == true)
    {
        MT4ListenerError error(std::string("Already Start Listening!"));
        throw(error);
    }

    hp = gethostbyname(host_name.c_str());
    ip_addr = inet_ntop(AF_INET, hp->h_addr, server_ip, 20);

    if( NULL == ip_addr)
    {
        MT4ListenerError error(std::string(strerror(errno)));
        throw(error);
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr))<0)
    {
        MT4ListenerError error(std::string(strerror(errno)));
        throw(error);
    }

    if(bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1){
        MT4ListenerError error(std::string(strerror(errno)));
        throw(error);
    }

    if(listen(sock, 30) == -1)
    {
        MT4ListenerError error(std::string(strerror(errno)));
        throw(error);
    }

    listening = true;
}

int MT4Listener::accept_conn()
{
    struct sockaddr_in cli_addr;
    socklen_t length = sizeof(cli_addr);

    if(listening == false)
        return -1;

    return accept( sock, (struct sockaddr*)&cli_addr, &length);
}
