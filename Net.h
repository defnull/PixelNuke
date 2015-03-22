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
#include "UIWindow.h"

class Net {
public:
    Net();
    virtual ~Net();
    int watch(int port);
    void setCanvas(UIWindow canvas);
    struct event_base* getBase();
    void loop();
    void stop();
private:
    struct event_base* evbase;
    std::list<NetSession*> sessions;
    bool running = true;
    UIWindow canvas;
};

#endif	/* NET_H */

