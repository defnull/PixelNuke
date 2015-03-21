/* 
 * File:   Net.h
 * Author: marc
 *
 * Created on March 20, 2015, 3:38 PM
 */

#ifndef NET_H
#define	NET_H

#include <list>
#include <event2/event.h>
#include "NetSession.h"

class Net {
public:
    Net();
    Net(const Net& orig);
    virtual ~Net();
    int watch(int port);
    struct event_base* getBase();
    void loop();
    void stop();
private:
    struct event_base* evbase;
    std::list<NetSession*> sessions;
    bool running = true;
};

#endif	/* NET_H */

