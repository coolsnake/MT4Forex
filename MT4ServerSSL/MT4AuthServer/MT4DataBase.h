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

#include "MT4Error.h"
#include "MT4Debug.h"


class MT4Database
{
public:
    MT4Database();
    ~MT4Database();

    int connect(const std::string& host, const std::string& username, const std::string& password, const std::string& database );
    bool user_auth_query(const std::string& username, const std::string& password, long EAID);

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
