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

static const struct timeval DEAD_TIMEOUT = {.tv_sec = 10, .tv_usec = 0};
static const struct timeval READ_TIMEOUT = {.tv_sec = 60, .tv_usec = 0};
#define SESSION_NEW   3
#define SESSION_ALIVE 2
#define SESSION_DYING 1
#define SESSION_DEAD  0
#define MAX_READBUFF 1024

class Net;

class NetSession {
public:
    NetSession(Net *net);
    NetSession(const NetSession& orig);
    virtual ~NetSession();
    void accept(evutil_socket_t sock);
    int mode = SESSION_NEW;
private:
    void onReadable();
    void onWriteable();
    void onError(short int which);
    void error(const char* msg);
    void close();
    Net *net;
    struct bufferevent *bevent;
    sockaddr_storage addr;
    struct timeval timeout = {.tv_sec=60, .tv_usec=0};
};

#endif	/* NETSESSION_H */

