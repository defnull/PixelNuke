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
#include "utils.h"

class Net : NonCopyable {
public:
    /// @throw std::runtime_error
    Net();
    ~Net();
    int watch(int port);
    event_base* getBase();
    void loop();
    void stop();
private:
    void remove_dead_sessions();
    event_base* evbase;
    std::vector< std::unique_ptr<NetSession> > sessions;
    bool running = true;
};

#endif	/* NET_H */

