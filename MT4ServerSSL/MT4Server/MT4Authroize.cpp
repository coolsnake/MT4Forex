#include "MT4Authroize.h"

MT4Authroize::MT4Authroize(const std::string& host, const std::string& username, const std::string& password, const std::string& database, MT4Operators *op, MT4Customers *cu)
{
    pthread_mutex_init(&queue_lock, NULL);
    auth_pass = 0xffff;
    auth_fail = 0x0000;
    running = true;
    Operators = op;
    Customers = cu;

    MT4_db = new MT4Database();
    try
    {
        MT4_db->connect(host, username, password, database);
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
        pthread_attr_setstacksize(&attr, 0x4000);
        pthread_create(&MT4_db_pid, &attr, MT4Database::DB_keep_alive, (void*)MT4_db);
    }
    catch(MT4Error &err)
    {
        err.show();
    }
}

MT4Authroize::~MT4Authroize()
{
    clear_pending_queue();
    stop_running();
    delete(MT4_db);
}

void MT4Authroize::clear_pending_queue()
{
    std::list<struct pend_socket>::iterator it;

    pthread_mutex_lock(&queue_lock);
    it = pending_queue.begin();
    while(it != pending_queue.end())
    {
        close((*it).sock);
        it = pending_queue.erase(it);
    }
    pthread_mutex_unlock(&queue_lock);
}


void * MT4Authroize::auth_check_thread(void *args)
{
    int max_select_sock = 0;
    int alive_sock = 0;
    struct timeval timeout;
    fd_set read_set;
    void *return_value = NULL;
    MT4Authroize *mt4_auth = static_cast<MT4Authroize*>(args);
    timeout.tv_sec = 0;
    timeout.tv_usec = 10000;
    while(mt4_auth->running)
    {
        pthread_mutex_lock(&mt4_auth->queue_lock);
        if(mt4_auth->pending_queue.empty())
        {
            pthread_mutex_unlock(&mt4_auth->queue_lock);
            usleep(10000);
            continue;
        }
        pthread_mutex_unlock(&mt4_auth->queue_lock);

        max_select_sock = mt4_auth->purge_pending_queue(&read_set);
        if(max_select_sock > 0)
        {
            max_select_sock ++;

            alive_sock = select(max_select_sock, &read_set, NULL, NULL, &timeout);
            if(alive_sock > 0)
            {
                mt4_auth->handle_auth_safe(&read_set);
            }
        }
    }
    return return_value;
}

void MT4Authroize::add(struct pend_socket sock)
{
    std::cout<<"add new socket to password check  "<<sock.sock<<std::endl;
    pthread_mutex_lock(&queue_lock);
    pending_queue.push_back(sock);
    pthread_mutex_unlock(&queue_lock);
}

int MT4Authroize::purge_pending_queue(fd_set* read_set)
{
    int max_sock = 0;
    int curr_time = time(NULL);
    std::list<struct pend_socket>::iterator it;
    FD_ZERO(read_set);

    pthread_mutex_lock(&queue_lock);
    it = pending_queue.begin();
    while(it != pending_queue.end())
    {
        if( curr_time - (*it).create_time > 5)
        {
            std::cout<<"Purge socket   "<<(*it).sock<<std::endl;
            SSL_shutdown((*it).ssl);
            SSL_free((*it).ssl);
            close((*it).sock);
            it = pending_queue.erase(it);
        }
        else
        {
            if((*it).sock > max_sock)
                max_sock = (*it).sock;
            FD_SET((*it).sock, read_set);
            it ++;
        }
    }
    pthread_mutex_unlock(&queue_lock);

    return max_sock;
}

int MT4Authroize::handle_auth_safe(fd_set *read_set)
{
    std::list<struct pend_socket>::iterator it;
    std::list<struct pend_socket>::iterator jt;
    std::list<struct pend_socket> temp;
    std::list<struct pend_socket> removed;
    int recv_length;
    int control_list[6];
    int control_num;
    int login_type;
    long status;
    struct auth_pair auth_data;

    temp.clear();
    removed.clear();

    pthread_mutex_lock(&queue_lock);
    it = pending_queue.begin();
    while( it != pending_queue.end() )
    {
        temp.push_back((*it));
        it++;
    }
    pthread_mutex_unlock(&queue_lock);


    it = temp.begin();
    while(it != temp.end())
    {
        if( FD_ISSET((*it).sock, read_set) )
        {
            memset(&auth_data, 0, sizeof(auth_data));

            recv_length = SSL_read((*it).ssl, &auth_data, MAX_AUTH_LENGTH);

            if( recv_length == sizeof(struct auth_pair) )
            {
                try{
                    login_type = MT4_db->customer_auth_check(std::string(auth_data.username), std::string(auth_data.password), control_list, &control_num);
                    std::cout<<"MT4Authroize check user account   "<<std::string(auth_data.username)<<std::endl;
                    if(login_type & OPERATOR_MASK)
                    {
                        if (SSL_write((*it).ssl, &auth_pass, sizeof(auth_pass)) == sizeof(auth_pass))
                        {
                        /*
                            some really stupid things, want to use blocked socket to make the life easier, but actually
                            it make life harder :-)
                            status = fcntl((*it).sock, F_GETFL, NULL);
                            fcntl((*it).sock, F_SETFL, status & (~O_NONBLOCK));
                         */
                            std::cout<<"MT4Authroize sock  "<<(*it).sock<<std::endl;
                            Operators->add_operator(control_list[0], std::string(auth_data.username), std::string(auth_data.password), (*it).ssl, (*it).sock);
                            removed.push_back((*it));
                        }
                    }
                    else if(login_type & CUSTOMER_MASK)
                    {
                        if (SSL_write((*it).ssl, &auth_pass, sizeof(auth_pass)) == sizeof(auth_pass))
                        {
                            Customers->add_customer(control_list[0], std::string(auth_data.username), std::string(auth_data.password), (*it).ssl, (*it).sock);
                            removed.push_back((*it));
                            for(int i=1;i<control_num;i++)
                            {
                                std::cout<<"Add customer to one operator's queue"<<std::endl;
                                Customers->add_customer_to_operator(control_list[0], control_list[i]);
                            }
                        }
                    }
                    else
                    {
                        SSL_write((*it).ssl, &auth_fail, sizeof(auth_fail));
                        SSL_shutdown((*it).ssl);
                        SSL_free((*it).ssl);
                        close((*it).sock);
                        removed.push_back((*it));
                    }
                }
                catch(MT4Error &err)
                {
                    err.show();
                }
            }
            else
            {
                close((*it).sock);
                removed.push_back((*it));
            }
        }
        it++;
    }

    pthread_mutex_lock(&queue_lock);
    jt = removed.begin();
    while( jt != removed.end() )
    {
        it = pending_queue.begin();
        while( it != pending_queue.end() )
        {
            if( (*jt).sock == (*it).sock &&
                (*jt).create_time == (*it).create_time )
            {
                std::cout<<"remove socket out from pending queue"<<"   "<<(*it).sock<<"   "<<std::endl;
                it = pending_queue.erase(it);
            }
            else
                it++;
        }
        jt++;
    }
    pthread_mutex_unlock(&queue_lock);

    return 0;
}

void MT4Authroize::stop_running()
{
    running = false;
}
