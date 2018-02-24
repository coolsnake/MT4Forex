#ifndef MT4DATABASE_H_INCLUDED
#define MT4DATABASE_H_INCLUDED

extern "C"
{
#include <pthread.h>
#include "mysql.h"
}

#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>

#include "MT4Error.h"

#define MASTER_MASK  0x08
#define NOR_CUSTOMER_MASK 0x01

class MT4Database
{
public:
    MT4Database();
    ~MT4Database();

    int connect(const std::string& host, const std::string& username, const std::string& password, const std::string& database );
    bool user_auth_query(const std::string& username, const std::string& password, long EAID);
    int customer_auth_check(const std::string& username, const std::string& password, int *control_list, int *num);

    static void *DB_keep_alive(void *args);

private:
    std::string DB_host;
    std::string DB_username;
    std::string DB_password;
    std::string DB_database;

    bool connected;
    pthread_mutex_t query_mutex;
    MYSQL *DB;
};

#endif // MT4DATABASE_H_INCLUDED
