#ifndef MT4DATATYPE_H_INCLUDED
#define MT4DATATYPE_H_INCLUDED

#define MAX_AUTH_LENGTH 48

extern "C" {
#include <openssl/ssl.h>
}

struct auth_pair
{
    char username[20];   //login user name
    char password[20];   // login password
};

struct Order
{
    char symbol[16];
    int cmd;   // buy or sell
    int slippage;
    int operate; // open or close
    int ticket; // ticket number
    double volume;  // the buying/selling volume
    double percent; // the percent of the buying/selling volume againt accountbalance
    double price;  // the contract price
    double stoploss; //
    double takeprofit;
};

struct pend_socket
{
    int sock;
    SSL *ssl;
    int create_time;
};
#endif // MT4DATATYPE_H_INCLUDED
