#include "mt4_client.h"
#include "mt4_datatype.h"


void *client_thread(void *arg)
{
    int sock_fd;
    int pass_status;
    struct sockaddr_in sa;
    sock_fd = socket(PF_INET, SOCK_STREAM, 0);

    memset( &sa, 0, sizeof(struct sockaddr_in) );
    sa.sin_family = AF_INET;
    sa.sin_port = htons(SERVER_PORT);
    sa.sin_addr.s_addr = inet_addr("60.191.3.145");

    if(connect(sock_fd, (struct sockaddr *)&sa, sizeof(sa)) < 0 )
    {
        printf("connect failed\n");
        return;
    };

    int r = rand() % 2;

    struct auth_pair auth;
    if(r == 0)
    {
        memcpy(auth.username, "fltz", 5);
        memcpy(auth.password, "fltz7788", 9);
    }
    if(r == 1)
    {
        memcpy(auth.username, "jrsn", 5);
        memcpy(auth.password, "lida1114", 9);
    }
    
    send(sock_fd, &auth, sizeof(auth), 0);
    recv(sock_fd, &pass_status, sizeof(int),0);
    if(pass_status == 0xFFFF)
        printf("Auth success %s\n",auth.username);
    else
        printf("Auth failed %s\n", auth.username);

    struct Order order;
    if(pass_status == 0xFFFF)
    {
        if(r == 0)
            strcpy(order.symbol, "XAUUSD");
        if(r == 1)
            strcpy(order.symbol, "EURUSD");
        if(r == 2)
            strcpy(order.symbol, "XAUUSD");
        order.cmd = 1;
        order.slippage = 2;
        order.volume = 200.3;
        order.price = 1982.23;
        order.stoploss = 1800.23;
        order.takeprofit = 2000.23;

        while(1)
        {
            if(send(sock_fd, &order, sizeof(struct Order), 0) < sizeof(struct Order))
            {
               printf("Channel has been closed %d\n",sock_fd);
               close(sock_fd);
               return ;
            }
            sleep(5);
        }
    }
}

int main()
{
    int i;
    pthread_t ntid;

    for(i=0;i<MAX_SOCKET;i++)
    {
        pthread_create(&ntid, NULL, client_thread, NULL);
        pthread_detach(ntid);

        usleep(100);
    }

    while(1)
    {
        sleep(1);
    }
}
