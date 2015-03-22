/* 
 * File:   NetSession.h
 * Author: marc
 *
 * Created on March 20, 2015, 3:57 PM
 */

#ifndef NETSESSION_H
#define	NETSESSION_H
#include <event2/event_struct.h>
#include <event2/bufferevent.h>
#include <event2/bufferevent_struct.h>
#include <ctime>
#include <string>
#include "utils.h"

static const timeval DEAD_TIMEOUT = {.tv_sec = 10, .tv_usec = 0};
static const timeval READ_TIMEOUT = {.tv_sec = 60, .tv_usec = 0};
#define SESSION_NEW   3
#define SESSION_ALIVE 2
#define SESSION_DYING 1
#define SESSION_DEAD  0
#define MAX_READBUFF 1024

class Net;

class NetSession : NonCopyable {
public:
    NetSession(Net *net, evutil_socket_t sock);
    ~NetSession();
    int mode = SESSION_NEW;
    void send(std::string msg);
    void send(const char *msg, size_t i);
private:
    void onReadable();
    void onWriteable();
    void onError(short int which);
    void error(const char* msg);
    void close();
    Net *net;
    bufferevent *bevent;
    sockaddr_storage addr;
    timeval timeout = {.tv_sec=60, .tv_usec=0};

};

#endif	/* NETSESSION_H */

