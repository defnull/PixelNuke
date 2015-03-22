/* 
 * File:   Net.cpp
 * Author: marc
 * 
 * Created on March 20, 2015, 3:38 PM
 */

#include "Net.h"
#include "NetSession.h"
#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/thread.h>
#include <cstring>

Net::Net() {
    evthread_use_pthreads();
    event_enable_debug_mode();
    evbase = event_base_new();
}

void Net::loop() {
    event_base_dispatch(evbase);
}

void Net::stop() {
    event_base_loopexit(evbase, NULL);
}

struct event_base* Net::getBase() {
    return evbase;
}

int Net::watch(int port) {
    struct event *listener_event;
    evutil_socket_t listener;

    struct sockaddr_in6 addr;
    listener = socket(AF_INET6, SOCK_STREAM, 0);
    memset(&addr, 0, sizeof (addr));
    addr.sin6_family = AF_INET6;
    addr.sin6_addr = in6addr_any;
    addr.sin6_port = htons(8080);

    if (listener < 0) {
        return 1;
    }

    evutil_make_socket_nonblocking(listener);
    evutil_make_listen_socket_reuseable(listener);

    if (bind(listener, (struct sockaddr*) &addr, sizeof (addr)) < 0)
        return 1;

    if (listen(listener, 16) < 0)
        return 1;

    listener_event = event_new(evbase, listener,
            EV_READ | EV_PERSIST, [] (evutil_socket_t listener, short event, void *arg) {
                Net *net = static_cast<Net*> (arg);
                NetSession *s = new NetSession(net);
                s->accept(listener);
                net->sessions.push_back(s);
                net->sessions.remove_if([] (const NetSession * value) {
                    return value->mode == SESSION_DEAD;
                });
            }, this);
    event_add(listener_event, NULL);
    return 0;
}

void Net::setCanvas(UIWindow canvas) {
    this->canvas = canvas;
}

Net::~Net() {
    stop();
}

