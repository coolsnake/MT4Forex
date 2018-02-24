#ifndef MT4_DATATYPE_H
#define MT4_DATATYPE_H

struct auth_pair
{
    char username[20];   //login user name
    char password[20];   // login password
};

#define STOP_SERVICE  1

#define BUFFER_MAX 256

struct MT4Command
{
    int cmd;
    int r_c;
    char buffer[BUFFER_MAX];
};

#endif
