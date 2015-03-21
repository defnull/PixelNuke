/* 
 * File:   NetSession.cpp
 * Author: marc
 * 
 * Created on March 20, 2015, 3:57 PM
 */

#include "NetSession.h"
#include "Net.h"

NetSession::NetSession(Net *net): net(net) {
}

NetSession::NetSession(const NetSession& orig) {
}

NetSession::~NetSession() {
}

void NetSession::accept(evutil_socket_t sockfd) {
    socklen_t slen = sizeof (addr);
    int fd;

    if ((fd = ::accept(sockfd, (struct sockaddr*) &addr, &slen)) < 0) {
        return;
    }
    
    evutil_make_socket_nonblocking(sockfd);
    bevent = bufferevent_socket_new(this->net->getBase(), sockfd, BEV_OPT_CLOSE_ON_FREE);

    bufferevent_setcb(bevent,
        [] (struct bufferevent *bev, void *ctx) {
            static_cast<NetSession*>(ctx)->onReadable();
        },
        [] (struct bufferevent *bev, void *ctx) {
            static_cast<NetSession*>(ctx)->onWriteable();
        },
        [] (struct bufferevent *bev, short int which, void *ctx) {
            static_cast<NetSession*>(ctx)->onError(which);
        }
    , this);
    bufferevent_setwatermark(bevent, EV_READ, 0, 1024);
    bufferevent_set_timeouts(bevent, &timeout, NULL);
    bufferevent_enable(bevent, EV_READ | EV_WRITE);
}



void NetSession::onReadable() {

}

void NetSession::onWriteable() {

}

void NetSession::onError(short int which) {

}