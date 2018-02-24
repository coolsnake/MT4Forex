#ifndef MT4DEBUG_H_INCLUDED
#define MT4DEBUG_H_INCLUDED

#include <syslog.h>
#include <string>
#include <iostream>

extern void DEBUG_ON();
extern void DEBUG_OFF();
extern void DEBUG(const std::string &content);

#endif
