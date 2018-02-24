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
        memcpy(auth.username, "alex", 5);
        auth.EAID = 1;
    }
/*
    if(r == 1)
        memcpy(auth.username, "isabel", 7);
*/
    if(r == 1)
    {
        memcpy(auth.username, "fred", 5);
        auth.EAID = 2;
    }
    memcpy(auth.password, "840601", 7);
    send(sock_fd, &auth, sizeof(auth), 0);
    recv(sock_fd, &pass_status, sizeof(int),0);
    if(pass_status == 0xFFFF)
        printf("Auth success %s\n",auth.username);
    else
        printf("Auth failed %s\n", auth.username);

    if(pass_status == 0xFFFF)
    {
        struct auth_keep_alive keep_alive;
        strcpy(keep_alive.auth,auth.username);
        while(1)
        {
            if(send(sock_fd, &keep_alive.auth, sizeof(struct auth_keep_alive), 0) < sizeof(struct auth_keep_alive))
            {
               printf("Channel has been closed %d\n",sock_fd);
               close(sock_fd);
               return ;
            }
	    else
            {
                if(recv(sock_fd, &keep_alive.auth, sizeof(struct auth_keep_alive),0)<sizeof(struct auth_keep_alive))
		{
		    printf("received error\n");
                    close(sock_fd);
                    return ;
		}
                else
                {
                    printf("the received auth %s\n",keep_alive.auth);
                }
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
