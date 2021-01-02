/*
 * File:   NetSession.h
 * Author: marc
 *
 * Created on March 20, 2015, 3:57 PM
 */

#ifndef NETSESSION_H
#define	NETSESSION_H
#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/bufferevent_struct.h>
#include <ctime>
#include <string>
#include <memory>
#include "utils.h"
#include "PxCommand.h"
#include <thread>
#include <stdexcept>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <array>


static const timeval DEAD_TIMEOUT = {10, 0};
static const timeval READ_TIMEOUT = {60, 0};
#define SESSION_NEW   3
#define SESSION_ALIVE 2
#define SESSION_DYING 1
#define SESSION_DEAD  0


using IPv6 = std::array<unsigned char, 16>;

template<typename UT>
class Net;

template<typename UT>
class NetSession : NonCopyable
{
public:
    NetSession ( Net<UT> *net, evutil_socket_t sock );
    ~NetSession();
    int mode = SESSION_NEW;
    void send ( const std::string &msg, bool priority = true ) const;
    void send ( const char *msg, size_t n, bool priority = true ) const;
    void error ( const char* msg );
    void close();
    IPv6 getIp();
    std::shared_ptr<UT> data;
    Net<UT> &getServer();
    sockaddr_storage addr;
private:
    void onReadable();
    void onWriteable();
    void onError ( short int which );
    Net<UT> *net = NULL;
    bufferevent *bevent = NULL;
    timeval timeout = {60, 0};
    size_t bufferSize = 10240;
    size_t maxLine = 1024;
    PxCommand currentCommand;
};

template<typename UT>
inline NetSession<UT>::NetSession ( Net<UT>* net, evutil_socket_t sockfd ) : net ( net )
{
    socklen_t slen = sizeof ( addr );
    int csockfd;

    if ( ( csockfd = ::accept ( sockfd, ( sockaddr* ) &addr, &slen ) ) < 0 ) {
        mode = SESSION_DEAD;
        return;
    }

    printf ( "Network: New connection\n" );

    mode = SESSION_ALIVE;

    evutil_make_socket_nonblocking ( csockfd );
    bevent = bufferevent_socket_new ( this->net->getBase(), csockfd, BEV_OPT_CLOSE_ON_FREE );

    bufferevent_setcb ( bevent,
    [] ( bufferevent *bev, void *ctx ) {
        static_cast<NetSession*> ( ctx )->onReadable();
    },
    [] ( bufferevent *bev, void *ctx ) {
        static_cast<NetSession*> ( ctx )->onWriteable();
    },
    [] ( bufferevent *bev, short int which, void *ctx ) {
        static_cast<NetSession*> ( ctx )->onError ( which );
    }
    , this );
    bufferevent_setwatermark ( bevent, EV_READ, 0, bufferSize );
    bufferevent_set_timeouts ( bevent, &timeout, NULL );
    bufferevent_enable ( bevent, EV_READ | EV_WRITE );
    net->onConnect ( *this );
}

template<typename UT>
inline NetSession<UT>::~NetSession()
{
    net->onDisconnect ( *this );
    if ( bevent ) {
        bufferevent_free ( bevent );
    }
}

template<typename UT>
inline void NetSession<UT>::send ( const std::string& msg, bool priority ) const
{
    send ( msg.c_str(), msg.length(), priority );
}

template<typename UT>
inline void NetSession<UT>::send ( const char* msg, size_t n, bool priority ) const
{
    if ( priority || evbuffer_get_length ( bufferevent_get_output ( bevent ) ) < bufferSize ) {
        bufferevent_write ( bevent, msg, n );
        bufferevent_write ( bevent, "\n", 1 );
    } else {
        auto clientAddr = ( ( sockaddr_in6* ) & ( addr ) )->sin6_addr;
        char ip[64];
        inet_ntop ( PF_INET6, ( struct in_addr6* ) & ( clientAddr ), ip, sizeof ( ip )-1 );
        printf ( "Network: Dropped line for %s (buffer full)\n", ip);
    }
}

template<typename UT>
inline void NetSession<UT>::error ( const char* msg )
{
    if ( mode == SESSION_DEAD ) {
        return;
    }
    evbuffer_add_printf ( bufferevent_get_output ( bevent ), "ERR %s\n", msg );
    close();
}

template<typename UT>
inline IPv6 NetSession<UT>::getIp()
{
    IPv6 v6;
    memcpy ( v6.data(), & ( ( sockaddr_in6* ) &addr )->sin6_addr, 16 );
    return v6;
}

template<typename UT>
inline void NetSession<UT>::close()
{
    if ( mode == SESSION_NEW ) {
        mode = SESSION_DEAD;
    } else if ( mode == SESSION_ALIVE ) {
        mode = SESSION_DYING;
        bufferevent_disable ( bevent, EV_READ );
        bufferevent_flush ( bevent, EV_WRITE, BEV_FINISHED );

        if ( evbuffer_get_length ( bufferevent_get_output ( bevent ) ) == 0 ) {
            bufferevent_free ( bevent );
            bevent = NULL;
            mode = SESSION_DEAD;
        } else {
            bufferevent_set_timeouts ( bevent, &DEAD_TIMEOUT, &DEAD_TIMEOUT );
            bufferevent_setcb ( bevent, NULL,
            // Write Callback
            [] ( bufferevent *bev, void *ctx ) {
                if ( evbuffer_get_length ( bufferevent_get_output ( bev ) ) == 0 ) {
                    NetSession *me = static_cast<NetSession*> ( ctx );
                    bufferevent_free ( me->bevent );
                    me->bevent = NULL;
                    me->mode = SESSION_DEAD;
                }
            },
            // Error callback
            [] ( bufferevent *bev, short int which, void *ctx ) {
                NetSession *me = static_cast<NetSession*> ( ctx );
                bufferevent_free ( me->bevent );
                me->bevent = NULL;
                me->mode = SESSION_DEAD;
            }
            , this );
        }
    }
}

template<typename UT>
inline Net<UT>& NetSession<UT>::getServer()
{
    return *net;

}

template<typename UT>
inline void NetSession<UT>::onReadable()
{
    char *line;
    size_t n;
    auto *input = bufferevent_get_input ( bevent );

    while ( ( line = evbuffer_readln ( input, &n, EVBUFFER_EOL_LF ) ) ) {
        currentCommand.set ( line, n );
        net->fireCallback ( *this, currentCommand );
    }

    if ( evbuffer_get_length ( input ) >= maxLine ) {
        error ( "Line to long" );
    }
}

template<typename UT>
inline void NetSession<UT>::onWriteable()
{
}

template<typename UT>
inline void NetSession<UT>::onError ( short int which )
{
    close();

}

#endif	/* NETSESSION_H */

