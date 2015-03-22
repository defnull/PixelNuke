/* 
 * File:   NetSession.cpp
 * Author: marc
 * 
 * Created on March 20, 2015, 3:57 PM
 */

#include "NetSession.h"
#include "Net.h"
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <string>
#include <glob.h>

NetSession::NetSession(Net *net, evutil_socket_t sockfd) : net(net) {
    socklen_t slen = sizeof (addr);
    int csockfd;
    
    if ((csockfd = ::accept(sockfd, (sockaddr*) &addr, &slen)) < 0) {
        mode = SESSION_DEAD;
        return;
    }
    
    mode = SESSION_ALIVE;

    evutil_make_socket_nonblocking(csockfd);
    bevent = bufferevent_socket_new(this->net->getBase(), csockfd, BEV_OPT_CLOSE_ON_FREE);

    bufferevent_setcb(bevent,
            [&] (bufferevent *bev, void *ctx) {
                static_cast<NetSession*> (ctx)->onReadable();
            },
    [&] (bufferevent *bev, void *ctx) {
        static_cast<NetSession*> (ctx)->onWriteable();
    },
    [&] (bufferevent *bev, short int which, void *ctx) {
        static_cast<NetSession*> (ctx)->onError(which);
    }
    , this);
    bufferevent_setwatermark(bevent, EV_READ, 0, 1024);
    bufferevent_set_timeouts(bevent, &timeout, NULL);
    bufferevent_enable(bevent, EV_READ | EV_WRITE);
}

NetSession::~NetSession() {
    bufferevent_free(bevent);
}

void NetSession::onReadable() {
    char *line;
    size_t n;
    auto *input = bufferevent_get_input(bevent);

    while ((line = evbuffer_readln(input, &n, EVBUFFER_EOL_LF))) {
        unsigned int x, y, c, t1, t2;
        int m = sscanf(line, "PX %u %u %n%8x%n", &x, &y, &t1, &c, &t2);
        if (m == 2) {
            printf("R %d %d", x, y);
        } else if (m == 3 && t2 - t1 == 6) {
            c |= 0xff000000;
            printf("PX %u %u %x", x, y, c);
        } else if (m == 3 && t2 - t1 == 8) {
            // #rrggbbaa -> #aarrggbb
            c = (c >> 8) + ((c & 0xff) << 24);
            printf("PX %d %d %x", x, y, c);
        }

        send(line, n);
        send("\n", 1);
        delete line;
    }

    if (evbuffer_get_length(input) >= 1024) {
        error("Line to long");
    }
}

void NetSession::onWriteable() {

}

void NetSession::onError(short int which) {
    close();
}

void NetSession::close() {
    if (mode == SESSION_NEW) {
        mode = SESSION_DEAD;
    } else if (mode == SESSION_ALIVE) {
        mode = SESSION_DYING;
        bufferevent_disable(bevent, EV_READ);
        bufferevent_flush(bevent, EV_WRITE, BEV_FINISHED);        
        
        if (evbuffer_get_length(bufferevent_get_output(bevent)) == 0) {
            bufferevent_free(bevent);
            mode = SESSION_DEAD;
        } else {
            bufferevent_set_timeouts(bevent, &DEAD_TIMEOUT, &DEAD_TIMEOUT);
            bufferevent_setcb(bevent, NULL,
                    // Write Callback
                    [] (bufferevent *bev, void *ctx) {
                        if (evbuffer_get_length(bufferevent_get_output(bev)) == 0) {
                            NetSession *me = static_cast<NetSession*> (ctx);
                            bufferevent_free(me->bevent);
                            me->mode = SESSION_DEAD;
                        }
                    },
            // Error callback
            [] (bufferevent *bev, short int which, void *ctx) {
                NetSession *me = static_cast<NetSession*> (ctx);
                bufferevent_free(me->bevent);
                me->mode = SESSION_DEAD;
            }
            , this);
        }
    }
}

void NetSession::error(const char * msg) {
    evbuffer_add_printf(bufferevent_get_output(bevent), "ERR %s\n", msg);
    close();
}

void NetSession::send(std::string msg) {
    send(msg.c_str(), msg.length());
}

void NetSession::send(const char *msg, size_t n) {
    bufferevent_write(bevent, msg, n);
}
