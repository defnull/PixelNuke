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

class Net;

class NetSession {
public:
    NetSession(Net *net);
    NetSession(const NetSession& orig);
    virtual ~NetSession();
    void accept(evutil_socket_t sock);
private:
    void onReadable();
    void onWriteable();
    void onError(short int which);
    Net *net;
    struct bufferevent *bevent;
    sockaddr_storage addr;
    struct timeval timeout = {.tv_sec=60, .tv_usec=0};
};

#endif	/* NETSESSION_H */

