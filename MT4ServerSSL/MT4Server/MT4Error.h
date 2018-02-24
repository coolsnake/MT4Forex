#ifndef MT4ERROR_H_INCLUDED
#define MT4ERROR_H_INCLUDED

#include <string>

extern "C"
{
#include <syslog.h>
}

class MT4Error{
public:
    virtual void show() = 0;
};

class MT4ListenerError : public MT4Error{
public:
    MT4ListenerError(const std::string& err)
    {
        ErrorMsg = std::string("MT4Listener:  ")+err;
    };

    void show()
    {
        syslog(LOG_USER|LOG_INFO, ErrorMsg.c_str());
    };
private:
    std::string ErrorMsg;
};

class MT4DatabaseError : public MT4Error{
public:
    MT4DatabaseError(const std::string& err)
    {
        ErrorMsg = std::string("MT4Database:  ")+err;
    };

    void show()
    {
        syslog(LOG_USER|LOG_INFO, ErrorMsg.c_str());
    };
private:
    std::string ErrorMsg;
};

#endif // MT4ERROR_H_INCLUDED
