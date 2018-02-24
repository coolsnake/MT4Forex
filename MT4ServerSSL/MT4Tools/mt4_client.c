#include "mt4_client.h"
#include "mt4_datatype.h"

int main()
{
   int sock_fd;
   struct auth_pair auth;

   struct sockaddr_in sa;
   sock_fd = socket(PF_INET, SOCK_STREAM, 0);

   memset( &sa, 0, sizeof(struct sockaddr_in) );
   sa.sin_family = AF_INET;
   sa.sin_port = htons(SERVER_PORT);
   sa.sin_addr.s_addr = inet_addr("115.236.18.118");

   if(connect(sock_fd, (struct sockaddr *)&sa, sizeof(sa)) < 0 )
   {
       printf("connect failed\n");
       return;
   };

   memset(&auth,0,sizeof(struct auth_pair));
   memcpy(auth.username, "alex", 5);
   memcpy(auth.password, "az840601", 9);

   send(sock_fd, &auth, sizeof(auth), 0);

   struct MT4Command mt4_cmd;
   memset(&mt4_cmd, 0, sizeof(struct MT4Command));

   mt4_cmd.cmd = 1;

   send(sock_fd, &mt4_cmd, sizeof(struct MT4Command), 0);
   close(sock_fd);

   return(0);
}
