#ifndef MT4AUTHCLUSTER_H_INCLUDED
#define MT4AUTHCLUSTER_H_INCLUDED

#include <iostream>
#include <vector>

extern "C"
{
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
}

#include "MT4Authroize.h"
#include "MT4SSLAuth.h"
#include "MT4DataBase.h"
#include "MT4Operators.h"
#include "MT4Customers.h"
#include "MT4Error.h"

class MT4AuthCluster
{
public:
    MT4AuthCluster(int num, const std::string &pu_key, const std::string &pr_key, const std::string& host, const std::string& username, const std::string& password, const std::string& database, MT4Operators *op, MT4Customers *cu);
    ~MT4AuthCluster();

    void start_auth();
    void add(int sock);

private:
    int get_current_pos();

private:
    int cluster_num;
    int current_position;
    std::vector<MT4SSLAuth*> MT4Auth;
    std::string DB_host;
    std::string DB_username;
    std::string DB_password;
    std::string DB_database;
    std::string public_key;
    std::string private_key;
    MT4Operators *Operators;
    MT4Customers *Customers;
};

#endif // MT4AUTHCLUSTER_H_INCLUDED
