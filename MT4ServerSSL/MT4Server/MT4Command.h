#ifndef MT4ACOMMAND_H_INCLUDED
#define MT4ACOMMAND_H_INCLUDED

#define STOP_SERVICE  1

#define BUFFER_MAX 256

struct MT4Command
{
    int cmd;
    int r_c;
    char buffer[BUFFER_MAX];
};

#endif
