#include "MT4Communicator.h"

MT4Communicator::MT4Communicator(const std::string& hostname, int port, const std::string& name, const std::string& password)
{
    listening = false;
    auth_name = name;
    auth_password = password;

    sock = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    struct hostent *hp;
    const char *ip_addr;
    char server_ip[20];
    int reuse_addr = 1;

    hp = gethostbyname(hostname.c_str());
    ip_addr = inet_ntop(AF_INET, hp->h_addr, server_ip, 20);

    if ( NULL == ip_addr) {
        return;
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr))<0) {
        return;
    }

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        return;
    }

    if (listen(sock, 2) == -1) {
        return;
    }

    listening = true;
}

void MT4Communicator::command_parse(const struct MT4Command &mt4_cmd)
{
    switch(mt4_cmd.cmd)
    {
        case 1: // stop this process
            exit(-1);
            break;
        default://default command
            break;
    }

}

/* one connection only, just used for communicate with control pannel, single access. need
   define command structure and result then to get the statistic data of the MT4Server,
   and even dynamic set parameter of the server.
*/
void *MT4Communicator::communicator_thread(void *arg)
{
    MT4Communicator *cm = static_cast<MT4Communicator*>(arg);
    int cli_sock;
    struct sockaddr_in cli_addr;
    socklen_t length = sizeof(cli_addr);
    int recv_len;
    struct auth_pair auth_data;
    struct MT4Command mt4_cmd;

    while (cm->listening) {
        cli_sock = accept( cm->sock, (struct sockaddr*)&cli_addr, &length );
        recv_len = recv( cli_sock, &auth_data, MAX_AUTH_LENGTH, 0 );
        if ( recv_len == sizeof(struct auth_pair)) {
            if (!strcmp(auth_data.username, cm->auth_name.c_str()) && !strcmp(auth_data.password, cm->auth_password.c_str()))
            {
                while(recv_len = recv( cli_sock, &mt4_cmd, sizeof(struct MT4Command), 0) > 0)
                {
                    cm->command_parse(mt4_cmd);
                }
                close(cli_sock);
            }
            else
                close(cli_sock);
        }
    }
}
