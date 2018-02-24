#include <iostream>

extern "C" {
#include <openssl/ssl.h>
#include <openssl/err.h>
}

#include "MT4Listener.h"
#include "MT4AuthCluster.h"
#include "MT4Error.h"
#include "MT4Debug.h"

#define MAX_PENDING_QUEUE 3


using namespace std;


int main()
{

    if(fork()!=0) return(1);
    if(setsid()<0) return(1);

    pid_t child_pid;

restarting:

    if( (child_pid = fork()) == 0)
    {
        DEBUG_ON();

        SSL_library_init();
        OpenSSL_add_ssl_algorithms();
        SSL_load_error_strings();
        MT4Listener MT4_Listener("soft.em7788.com");
        MT4AuthCluster MT4_Cluster(MAX_PENDING_QUEUE, "cacert.pem", "privkey.pem", "soft.em7788.com", "mt4", "840601", "mt4_accounts");

        try{
            MT4_Listener.start_listen();
            DEBUG("Start listen ready");
            MT4_Cluster.start_auth();
            DEBUG("Thread creation success");
        }
        catch(MT4Error &err)
        {
            err.show();
            return 0;
        }

        while(true)
        {
            int cli_sock;
            cli_sock = MT4_Listener.accept_conn();
            MT4_Cluster.add(cli_sock);
        }
    }
    else
    {
        pid_t w;
        int status;
        w = waitpid(child_pid,&status,WUNTRACED | WCONTINUED);
        sleep(5);
        goto restarting;
    }
    return 0;
}
