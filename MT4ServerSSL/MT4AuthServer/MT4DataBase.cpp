#include "MT4DataBase.h"

MT4Database::MT4Database()
{
    connected = false;
    pthread_mutex_init(&query_mutex, NULL);
    DB = mysql_init(NULL);
    if(DB == NULL)
    {
        MT4DatabaseError err(std::string("Initialization Database failed"));
        throw(err);
    }
}

MT4Database::~MT4Database()
{
    if(connected)
        mysql_close(DB);
}

int MT4Database::connect(const std::string& host, const std::string& username, const std::string& password, const std::string& database)
{
    DB_host = host;
    DB_username = username;
    DB_password = password;
    DB_database = database;

    if(DB == NULL)
    {
        MT4DatabaseError err(std::string("Uninitialized DB"));
        throw(err);
    }

    if(!mysql_real_connect(DB, DB_host.c_str(), DB_username.c_str(), DB_password.c_str(), DB_database.c_str(), 0, NULL, 0))
    {
        MT4DatabaseError err(std::string(mysql_error(DB)));
        throw(err);
    }

    my_bool enable = 1;
    mysql_options(DB, MYSQL_OPT_RECONNECT, &enable);

    connected = true;
    return 0;
}

void *MT4Database::DB_keep_alive(void *args)
{

    MT4Database *MT4_DB = static_cast<MT4Database*>(args);
    unsigned long mysql_id_old, mysql_id_new;

    while(1)
    {
        pthread_mutex_lock(&MT4_DB->query_mutex);
        mysql_id_old = mysql_thread_id(MT4_DB->DB);
        mysql_ping(MT4_DB->DB);
        mysql_id_new = mysql_thread_id(MT4_DB->DB);
        pthread_mutex_unlock(&MT4_DB->query_mutex);
        sleep(30);
    }
}

bool MT4Database::user_auth_query(const std::string& username, const std::string& password, long EAID)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    std::string query;
    std::string id;
    bool return_value = false;

    std::stringstream stream;
    stream<<EAID;
    stream>>id;

    if(DB == NULL)
    {
        MT4DatabaseError err(std::string("Uninitialized DB"));
        throw(err);
    }

    query = "select password from mt4_accounts.permision where username =\""+username+"\""+" and EAID="+id;

    DEBUG(query);

    pthread_mutex_lock(&query_mutex);
    if(!mysql_query(DB, query.c_str()))
    {
        res = mysql_use_result(DB);
        if( res != NULL )
        {
            row = mysql_fetch_row(res);
            if( row && row[0] )
            {
                if(!password.compare(row[0]))
                    return_value = true;
                else
                    return_value = false;
            }
            else
            {
                mysql_free_result(res);
                pthread_mutex_unlock(&query_mutex);
                MT4DatabaseError err(std::string("Noexisted username   ") + username );
                throw(err);
            }
            mysql_free_result(res);
        }
        else
        {
            pthread_mutex_unlock(&query_mutex);
            MT4DatabaseError err(std::string("No result find from DB"));
            throw(err);
        }
    }
    else
    {
        pthread_mutex_unlock(&query_mutex);
        MT4DatabaseError err(std::string(mysql_error(DB)));
        throw(err);
    }
    pthread_mutex_unlock(&query_mutex);
    return return_value;
}

