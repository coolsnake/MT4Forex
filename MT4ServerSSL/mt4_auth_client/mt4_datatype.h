#ifndef MT4_DATATYPE_H
#define MT4_DATATYPE_H

struct auth_pair
{
    char username[20];   //login user name
    char password[20];   // login password
    long EAID;
};

struct Order
{
    char symbol[16];
    int cmd;   // buy or sell
    int slippage;
    double volume;  // the percent of buying volume
    double price;  // the contract price
    double stoploss; //
    double takeprofit;
    int operate; // open or close
    int ticket; // ticket number
};

struct auth_keep_alive
{
    char auth[16];
};

struct server_statistic
{
    int online_customer;
    int online_operator;

};
#endif
