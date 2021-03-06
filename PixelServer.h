/*
 * File:   PixelServer.h
 * Author: marc
 *
 * Created on March 22, 2015, 2:48 PM
 */

#ifndef PIXELSERVER_H
#define	PIXELSERVER_H

#include "UIWindow.h"
#include "Net.h"
#include <thread>
#include "utils.h"
#include <map>

class PixelSession;

class PixelServer : NonCopyable
{
public:
    PixelServer();
    ~PixelServer();
    void run();
    void stop();

private:
    UIWindow window;
    UILayer pxLayer;
    UILayer guiLayer;
    Net<PixelSession> server;
    std::map<IPv6, std::shared_ptr<PixelSession>> users;
    bool running;
};

#endif	/* PIXELSERVER_H */

