#include "MT4Debug.h"

bool debug_on;

void DEBUG_ON()
{
    debug_on = true;
};

void DEBUG_OFF()
{
    debug_on = false;
};

void DEBUG(const std::string &content)
{
    if(debug_on)
        std::cout<<content<<std::endl;
};

void LOG(const std::string &content)
{
    syslog(LOG_USER,content.c_str());
}
