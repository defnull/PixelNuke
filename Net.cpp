/* 
 * File:   Net.cpp
 * Author: marc
 * 
 * Created on March 20, 2015, 3:38 PM
 */

#include <stdexcept>
#include <algorithm>
#include <cstring>
#include <iostream>
#include "Net.h"
#include "NetSession.h"
#include "event2/event.h"
#include "event2/event_struct.h"
#include "event2/thread.h"

Net::Net() {
    if (evthread_use_pthreads() != 0)
        throw std::runtime_error("Could not set up libevent for use with Pthreads");
    evbase = event_base_new();
    if (!evbase)
        throw std::runtime_error("Could not create new event_base via libevent");
}

void Net::setCallback(const std::string &name, const netCallback& cb) {
    callbacks[name] = cb;
}

void Net::fireCallback(PxCommand &cmd) {
    try {
        callbacks.at(cmd.get(0))(cmd);
    } catch (std::out_of_range) {
        cmd.getClient().error("Unknown command");
    } catch (PxParseError const& x) {
    	cmd.getClient().error(x.what());
	} catch (std::exception const& x) {
		std::cerr << "Exception: " << x.what() << std::endl;
	}
}

void Net::loop() {
	printf("Network: Starting...\n");
    event_base_dispatch(evbase);
	printf("Network: Stopped.\n");
}

void Net::stop() {
	printf("Network: stop\n");
    event_base_loopexit(evbase, NULL);
}

event_base* Net::getBase() {
    return evbase;
}

void Net::remove_dead_sessions() {
    auto new_end = std::remove_if(
        sessions.begin(),
        sessions.end(),
        [] (std::unique_ptr<NetSession> const& value) {
            return value->mode == SESSION_DEAD;
        });
    sessions.erase(new_end, sessions.end());
}

int Net::watch(int port) {
    evutil_socket_t listener;

    sockaddr_in6 addr;
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

    if (bind(listener, (sockaddr*) &addr, sizeof (addr)) < 0)
        return 1;

    if (listen(listener, 16) < 0)
        return 1;

    auto* listener_event = event_new(evbase, listener,
            EV_READ | EV_PERSIST, [] (evutil_socket_t listener, short event, void *arg) {
                Net *net = static_cast<Net*>(arg);
                std::unique_ptr<NetSession> s (new NetSession(net, listener));
                net->remove_dead_sessions();
                net->sessions.push_back(std::move(s));
            }, this);
    event_add(listener_event, NULL);

    return 0;
}

Net::~Net() {
    stop();
    event_base_loopbreak(evbase);
    event_base_free(evbase);
}

