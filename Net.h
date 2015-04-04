/* 
 * File:   Net.h
 * Author: marc
 *
 * Created on March 20, 2015, 3:38 PM
 */

#ifndef NET_H
#define	NET_H

#include <vector>
#include <memory>
#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/thread.h>
#include <stdexcept>
#include "NetSession.h"
#include "PxCommand.h"
#include "utils.h"
#include <unordered_map>
#include <functional>



template <typename UT>
class Net : NonCopyable {
public:
	typedef NetSession<UT> sessionType;
	typedef std::function<void(sessionType & session, PxCommand & cmd)> netCallback;
	typedef std::function<void(sessionType & session)> sessionCallback;

    Net();
    ~Net();
    int watch(int port);
    event_base* getBase();
    void loop();
    void detach();
    void stop();
    void setCallback(const std::string &name, const netCallback &cb);
    void fireCallback(sessionType & session, PxCommand &cmd);
    void setSessionCallbacks(const sessionCallback &connect, const sessionCallback &disconnect);
    std::vector< std::unique_ptr<sessionType> > &getSessions();
    sessionCallback onConnect = [](sessionType &sess){};
    sessionCallback onDisconnect = [](sessionType &sess){};
private:
    void remove_dead_sessions();
    event_base* evbase;
    std::vector< std::unique_ptr<sessionType> > sessions;
    bool running = true;
    std::unordered_map< std::string, netCallback > callbacks;
};

template<typename UT>
inline Net<UT>::Net() {
    if (evthread_use_pthreads() != 0)
        throw std::runtime_error("Could not set up libevent for use with Pthreads");
    evbase = event_base_new();
    if (!evbase)
        throw std::runtime_error("Could not create new event_base via libevent");
}

template<typename UT>
inline Net<UT>::~Net() {
    stop();
    event_base_loopbreak(evbase);
    event_base_free(evbase);
}

template<typename UT>
inline int Net<UT>::watch(int port) {
    evutil_socket_t listener;

    sockaddr_in6 addr;
    listener = socket(AF_INET6, SOCK_STREAM, 0);
    memset(&addr, 0, sizeof (addr));
    addr.sin6_family = AF_INET6;
    addr.sin6_addr = in6addr_any;
    addr.sin6_port = htons(port);

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
                Net<UT> *net = static_cast<Net<UT>*>(arg);
                std::unique_ptr<sessionType> s (new sessionType(net, listener));
                net->remove_dead_sessions();
                net->sessions.push_back(std::move(s));
            }, this);
    event_add(listener_event, NULL);

    return 0;
}

template<typename UT>
inline event_base* Net<UT>::getBase() {
    return evbase;
}

template<typename UT>
inline void Net<UT>::loop() {
	printf("Network: Starting...\n");
    event_base_dispatch(evbase);
	printf("Network: Stopped.\n");
}

template<typename UT>
inline void Net<UT>::detach() {
    std::thread thread ([&] {
    	loop();
    });
    thread.detach();
}

template<typename UT>
inline void Net<UT>::stop() {
	printf("Network: stop\n");
    event_base_loopexit(evbase, NULL);
}

template<typename UT>
inline void Net<UT>::setCallback(const std::string& name,
		const netCallback& cb) {
    callbacks[name] = cb;
}

template<typename UT>
inline void Net<UT>::fireCallback(sessionType& session, PxCommand& cmd) {
    try {
        callbacks.at(cmd.get(0))(session, cmd);
    } catch (std::out_of_range const &x) {
    	session.error("Unknown command");
    } catch (PxParseError const& x) {
    	session.error(x.what());
	} catch (std::exception const& x) {
		std::cerr << "Exception: " << x.what() << std::endl;
	}
}

template<typename UT>
inline void Net<UT>::setSessionCallbacks(const sessionCallback& connect,
		const sessionCallback& disconnect) {
	onConnect = connect;
	onDisconnect = disconnect;
}

template<typename UT>
inline std::vector<std::unique_ptr< typename Net<UT>::sessionType> >& Net<UT>::getSessions() {
	return sessions;
}

template<typename UT>
inline void Net<UT>::remove_dead_sessions() {
    auto new_end = std::remove_if(
        sessions.begin(),
        sessions.end(),
        [] (std::unique_ptr<sessionType> const& value) {
            return value->mode == SESSION_DEAD;
        });
    sessions.erase(new_end, sessions.end());
}

#endif	/* NET_H */

