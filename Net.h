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
#include "NetSession.h"
#include "PxCommand.h"
#include "utils.h"
#include <unordered_map>
#include <functional>

typedef std::function<void(PxCommand & cmd)> netCallback;
typedef std::function<void(NetSession *sess)> sessionCallback;

class Net : NonCopyable {
public:
    /// @throw std::runtime_error
    Net();
    ~Net();
    int watch(int port);
    event_base* getBase();
    void loop();
    void stop();
    void setCallback(const std::string &name, const netCallback &cb);
    void fireCallback(PxCommand &cmd);
    void setSessionCallbacks(const sessionCallback &connect, const sessionCallback &disconnect);
    std::vector< std::unique_ptr<NetSession> > &getSessions();
    sessionCallback onConnect = [](NetSession *sess){};
    sessionCallback onDisconnect = [](NetSession *sess){};
private:
    void remove_dead_sessions();
    event_base* evbase;
    std::vector< std::unique_ptr<NetSession> > sessions;
    bool running = true;
    std::unordered_map< std::string, netCallback > callbacks;
};

#endif	/* NET_H */

