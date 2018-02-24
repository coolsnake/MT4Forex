#include "MT4AuthCluster.h"

MT4AuthCluster::MT4AuthCluster(int num, const std::string& pu_key, const std::string& pr_key, const std::string& host, const std::string& username, const std::string& password, const std::string& database)
{
    cluster_num = num;
    current_position = 0;
    public_key = pu_key;
    private_key = pr_key;
    DB_host = host;
    DB_username = username;
    DB_password = password;
    DB_database = database;
    MT4_KA = new MT4KeepAlive;
}

MT4AuthCluster::~MT4AuthCluster()
{
    for(int i=0;i<cluster_num;i++)
        delete MT4Auth[i];
}

void MT4AuthCluster::start_auth()
{
    MT4Authroize *item_a;
    MT4SSLAuth *item_s;
    for(int i=0;i<cluster_num;i++)
    {
        item_a = new MT4Authroize(DB_host, DB_username, DB_password, DB_database, MT4_KA);
        item_s = new MT4SSLAuth(item_a, public_key, private_key);
        MT4Auth.push_back(item_s);
        pthread_t pid;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_attr_setstacksize(&attr, 0x4000);
        pthread_create(&pid, &attr, &MT4Authroize::auth_check_thread, (void*)item_a);
        pthread_create(&pid, &attr, &MT4SSLAuth::ssl_auth_thread, (void*)item_s);
    }

    pthread_t pid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attr, 0x4000);
    pthread_create(&pid, &attr, &MT4KeepAlive::keep_alive_thread, (void*)MT4_KA);
    DEBUG("MT4KeepAlive thread created success");
}

int MT4AuthCluster::get_current_pos()
{
    current_position = (current_position + 1) % cluster_num;
    return current_position;
}

void MT4AuthCluster::add(int sock)
{
    long status;
    status = fcntl(sock, F_GETFL, NULL);
    fcntl(sock, F_SETFL, status | O_NONBLOCK);
    MT4Auth[get_current_pos()]->add(sock);
}
