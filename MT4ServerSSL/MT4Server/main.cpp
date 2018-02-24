#include <iostream>
#include <string.h>
#include <time.h>

extern "C" {
#include <openssl/ssl.h>
#include <openssl/err.h>
}

#include "MT4Listener.h"
#include "MT4AuthCluster.h"
#include "MT4Operators.h"
#include "MT4Customers.h"
#include "MT4Error.h"
#include "MT4Communicator.h"

#define MAX_PENDING_QUEUE 10


using namespace std;

string server_name = "soft.em7788.com";
string mysql_server_name = "soft.em7788.com";
string mysql_user_name = "mt4";
string mysql_user_password = "840601";
string control_sw = "on";
string control_server = "soft.em7788.com";
string control_user_name = "alex";
string control_user_password = "az840601";

#define HELP "-h"
#define SERVER_NAME "-s"
#define MYSQL_SERVER_NAME "-ms"
#define MYSQL_USER_NAME "-mun"
#define MYSQL_USER_PASSWORD "-mup"
#define CONTROL_SW "-c"
#define CONTROL_SERVER "-cs"
#define CONTROL_USER_NAME "-cu"
#define CONTROL_USER_PASSWORD "-cp"

MT4Communicator *Controller;

void help_content()
{
    std::cout<<"MT4 TRANSFER SERVER HELP CONTENT"<<std::endl;
    std::cout<<"-h For this help content"<<std::endl;
    std::cout<<"-s For change the mt4 transfer server hostname"<<std::endl;
    std::cout<<"-ms For change the mysql database server hostname"<<std::endl;
    std::cout<<"-mun For change the mysql database user name"<<std::endl;
    std::cout<<"-mup For change the mysql database user password"<<std::endl;
    std::cout<<"-c For starting the control pannel"<<std::endl;
    std::cout<<"-cs For change the controller server host"<<std::endl;
    std::cout<<"-cu For change the controller user name"<<std::endl;
    std::cout<<"-cp For change the controller user password"<<std::endl;
}

void arg_parser(int argc, char** argv)
{
    for(int i=0; i<argc; i++) {
        if (!strcmp(SERVER_NAME, argv[i])) {
            server_name = string(argv[i+1]);
        } else if (!strcmp(MYSQL_SERVER_NAME, argv[i])) {
            mysql_server_name = string(argv[i+1]);
        } else if (!strcmp(MYSQL_USER_NAME, argv[i])) {
            mysql_user_name = string(argv[i+1]);
        } else if (!strcmp(MYSQL_USER_PASSWORD, argv[i])) {
            mysql_user_password = string(argv[i+1]);
        } else if (!strcmp(CONTROL_SW, argv[i])) {
            control_sw = string(argv[i+1]);
        } else if (!strcmp(CONTROL_SERVER, argv[i])) {
            control_server = string(argv[i+1]);
        } else if (!strcmp(CONTROL_USER_NAME, argv[i])) {
            control_user_name = string(argv[i+1]);
        } else if (!strcmp(CONTROL_USER_PASSWORD, argv[i])) {
            control_user_password = string(argv[i+1]);
        } else if (!strcmp(HELP, argv[i]))
            help_content();
    }
}

bool control_pannel_daemon()
{
    Controller = new MT4Communicator(control_server,32001,control_user_name,control_user_password);

    pthread_t pid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 0x4000);

    if (pthread_create(&pid, &attr, &MT4Communicator::communicator_thread, (void*)(Controller)) == 0) {
        std::cout<<"Daemon of communication start...."<<std::endl;
        return true;
    } else {
        std::cout<<"Daemon of communication failed...."<<std::endl;
        return false;
    }
}

int main(int argc, char** argv)
{

    time_t current_time;
    time_t time_limit = 1302581324;

    arg_parser(argc, argv);

    if(fork()!=0) return(1);
    if(setsid()<0) return(1);

    pid_t child_pid;

restarting:

    if( (child_pid = fork()) == 0)
    {
        if (control_sw == "on") {
            if(!control_pannel_daemon())
                exit(-1);
        }

        SSL_library_init();
        OpenSSL_add_all_algorithms();
        SSL_load_error_strings();

        MT4Customers *MT4_Customers;
        MT4_Customers = new MT4Customers();

        MT4Operators *MT4_Operators;
        MT4_Operators = new MT4Operators(MT4_Customers);


        MT4Listener MT4_Listener("soft.em7788.com");
        MT4AuthCluster MT4_Cluster(2, "cacert.pem", "privkey.pem", "soft.em7788.com", "mt4", "840601", "mt4_accounts", MT4_Operators, MT4_Customers);

        try{
            MT4_Listener.start_listen();
            std::cout<<"Start listen ready"<<std::endl;
            MT4_Cluster.start_auth();
            std::cout<<"Thread creation success"<<std::endl;
        }
        catch(MT4Error &err)
        {
            err.show();
        }

        while(true)
        {
            int cli_sock;

            time(&current_time);
            if(current_time > time_limit)
                exit(-1);

            cli_sock = MT4_Listener.accept_conn();
            std::cout<<"Get one new connection from client"<<std::endl;
            MT4_Cluster.add(cli_sock);
        }
    }
    else
    {
        pid_t w;
        int status;
        w = waitpid(child_pid,&status,WUNTRACED | WCONTINUED);

        time(&current_time);
        if(current_time > time_limit)
            exit(-1);

        sleep(5);
        std::cout<<"MT4Server broken restart new thread"<<std::endl;
        goto restarting;
    }
    return 0;
}
