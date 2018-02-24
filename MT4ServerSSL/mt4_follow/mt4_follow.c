#include "mt4_datatype.h"
#include "mt4_follow.h"


void *client_thread(void *arg)
{
    int sock_fd;
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
    
    r = 0;
    struct auth_pair auth;
    if(r == 0)
    {
        memcpy(auth.username, "em001", 6);
        memcpy(auth.password, "em001", 6);
//        memcpy(auth.username, "lida", 5);
//        memcpy(auth.password, "lida88",7);
    }
    if(r == 1)
    {
        memcpy(auth.username, "em7788", 7);
        memcpy(auth.password, "em7788", 7);
    }

    int success;
    send(sock_fd, &auth, sizeof(auth), 0);
    recv(sock_fd, &success, sizeof(success), 0);
    printf("received result 0x%x\n", success);

    while(1)
    {
        struct Order order;
        int result;
        result = recv(sock_fd, &order, sizeof(order), 0);
        if(result == -1)
            printf("can be timeout\n");

        if(result == sizeof(order))
        {

            printf("the order's content symbol:  %s\n", order.symbol);
            printf("the order's content cmd: %d\n", order.cmd);
            printf("the order's content slippage %d\n", order.slippage);
            printf("the order's content volume %f\n", order.volume);
            printf("the order's content price %f\n", order.price);
            printf("the order's content stoploss %f\n", order.stoploss);
            printf("the order's content takeprofit %f\n", order.takeprofit);

            printf("One Order Received %d\n", (int)pthread_self());
        }
        else
        {
            return;
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

        usleep(10000);
    }

    while(1)
    {
        sleep(1);
    }
}
