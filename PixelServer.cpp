/*
 * File:   PixelServer.cpp
 * Author: marc
 *
 * Created on March 22, 2015, 2:48 PM
 */

#include "PixelServer.h"
#include "Net.h"
#include <thread>
#include <sstream>
#include <cstdio>
#include <ctime>
#include <arpa/inet.h>
#include <chrono>

struct PixelSession: NonCopyable {
    PixelSession() {
        gettimeofday ( &connected, NULL );
    };

    unsigned int pixel = 0;
    timeval connected;
    unsigned int lastpx[2] = {0,0};
    UILayer* drawTarget;
};

namespace
{
bool ipcmp ( sockaddr_storage *a, sockaddr_storage *b )
{
    if ( a->ss_family != b->ss_family ) {
        return false;
    }
    if ( a->ss_family == AF_INET ) {
        return 0 == std::memcmp ( & ( ( ( sockaddr_in* ) a )->sin_addr ),
                                  & ( ( ( sockaddr_in* ) b )->sin_addr ),
                                  sizeof ( in_addr ) );
    } else if ( a->ss_family == AF_INET6 ) {
        return 0 == std::memcmp ( & ( ( ( sockaddr_in6* ) a )->sin6_addr ),
                                  & ( ( ( sockaddr_in6* ) b )->sin6_addr ),
                                  sizeof ( in6_addr ) );
    }
    return false;
}
}


PixelServer::PixelServer() :
    window(),
    pxLayer ( 1024, false ),
    guiLayer ( 1024, true ),
    server()
{
    window.addLayer ( &pxLayer );
    window.addLayer ( &guiLayer );

    server.setSessionCallbacks (
    [&] ( NetSession<PixelSession> & session ) {
        auto ip = session.getIp();

        // Continue PixelSession is IP is known
        if ( users.find ( ip ) == users.end() ) {
            users[ip] = std::make_shared<PixelSession>();
        }
        session.data = users[ip];
        session.data->drawTarget = &pxLayer;
        // TODO: remove old sessions

        auto clientAddr = ( ( sockaddr_in6* ) & ( session.addr ) )->sin6_addr;
        char ips[64];
        inet_ntop ( PF_INET6, ( struct in_addr6* ) & ( clientAddr ), ips, sizeof ( ips )-1 );
        printf ( "Network: New connection %s\n", ips);

    },
    [&] ( NetSession<PixelSession> & session ) {
    } );

    server.setCallback ( "PX",
    [&] ( NetSession<PixelSession> & session, PxCommand &cmd ) {
        session.data->pixel += 1;
        if ( cmd.nargs() == 3 ) {
            GLuint x, y;
            cmd.parse ( 1, x );
            cmd.parse ( 2, y );

            char msg[64];
            int cx = snprintf ( msg, 64, "PX %d %d %08x", x, y, pxLayer.getPx ( x,y ) );
            if ( cx >= 0 && cx < 64 ) {
                session.send ( msg );
            }
        } else if ( cmd.nargs() == 4 ) {
            GLuint x,y,c;
            cmd.parse ( 1, x );
            cmd.parse ( 2, y );
            if ( cmd.len ( 3 ) == 6 ) {
                cmd.parse ( 3, c, 16 );
                c = ( c<<8 ) + 0xff;
            } else if ( cmd.len ( 3 ) == 8 ) {
                cmd.parse ( 3, c, 16 );
            } else {
                throw PxParseError ( "Color must have 3 or 4 byte components." );
            }
            session.data->drawTarget->setPx ( x, y, c );
        }
    } );

    server.setCallback ( "GUI",
    [&] ( NetSession<PixelSession> & session, PxCommand &cmd ) {
        session.data->drawTarget = &guiLayer;
    } );

    server.setCallback ( "SIZE",
    [&] ( NetSession<PixelSession> & session, PxCommand &cmd ) {
        if ( cmd.nargs() > 1 ) {
            throw PxParseError ( "SIZE does not take any parameters." );
        }
        std::ostringstream oss;
        oss << "SIZE " << window.width << " " << window.height;
        session.send ( oss.str() );
    } );

    server.setCallback ( "HELP",
    [&] ( NetSession<PixelSession> & session, PxCommand &cmd ) {
        session.send (
            "HELP: == PIXELFLUT ==\n"
            "HELP: Line based ASCII protocol\n"
            "HELP: Commands:\n"
            "HELP:   PX <x> <y> <RRGGBB>\n"
            "HELP:   PX <x> <y>\n"
            "HELP:   SIZE\n"
            "HELP: and more ;)" );
    } );

    server.setCallback ( "STAT",
    [&] ( NetSession<PixelSession> & session, PxCommand &cmd ) {
        for ( auto const & sess: session.getServer().getSessions() ) {
            auto addr = ( ( sockaddr_in6* ) & ( sess->addr ) )->sin6_addr;
            char ip[64];
            inet_ntop ( PF_INET6, ( struct in_addr6* ) & ( addr ), ip, sizeof ( ip )-1 );
            char msg[128];
            snprintf ( msg, 128, "STAT %s %u", ip, sess->data->pixel );
            session.send ( msg );
        }
    } );

    server.watch ( 1234 );
    server.detach();

    running = true;

    std::thread screenshotThread ( [&] {
        char filename[128];
        struct timeval tp;
        while ( running ) {
            gettimeofday ( &tp, NULL );
            long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000; //get current timestamp in milliseconds
            snprintf ( filename, 128, "log/px_%lu.png", ms );
            pxLayer.saveAs ( filename );
            std::this_thread::sleep_for ( std::chrono::seconds ( 1 ) );
        }
    } );
    screenshotThread.detach();
}

void PixelServer::run()
{
    window.loop();
}

void PixelServer::stop()
{
    running = false;
    server.stop();
    window.stop();
}

PixelServer::~PixelServer()
{
}

