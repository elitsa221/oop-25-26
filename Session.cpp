#define _CRT_SECURE_NO_WARNINGS
#include "Session.h"
#include <ctime>

Session& Session::instance() {
    static Session s;
    return s;
}

Session::Session() {
    
    time_t t = time(nullptr);
    tm* lt = localtime(&t);
    today = Date(lt->tm_mday, lt->tm_mon + 1, lt->tm_year + 1900);
}