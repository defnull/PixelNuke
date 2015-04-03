/* 
 * File:   NetSession.h
 * Author: marc
 *
 * Created on March 20, 2015, 3:57 PM
 */

#ifndef NETSESSION_H
#define	NETSESSION_H
#include "event2/event_struct.h"
#include "event2/bufferevent.h"
#include "event2/bufferevent_struct.h"
#include <ctime>
#include <string>
#include "utils.h"

static const timeval DEAD_TIMEOUT = {.tv_sec = 10, .tv_usec = 0};
static const timeval READ_TIMEOUT = {.tv_sec = 60, .tv_usec = 0};
#define SESSION_NEW   3
#define SESSION_ALIVE 2
#define SESSION_DYING 1
#define SESSION_DEAD  0

class Net;

class NetSession : NonCopyable {
public:
    NetSession(Net *net, evutil_socket_t sock);
    ~NetSession();
    int mode = SESSION_NEW;
    void send(const std::string &msg) const;
    void send(const char *msg, size_t i) const;
    void error(const char* msg);
    void close();
private:
    void onReadable();
    void onWriteable();
    void onError(short int which);
    Net *net = NULL;
    bufferevent *bevent = NULL;
    sockaddr_storage addr;
    timeval timeout = {.tv_sec=60, .tv_usec=0};
    size_t bufferSize = 1024;
    size_t maxLine = 1024;
};

#endif	/* NETSESSION_H */

