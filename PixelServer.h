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

class PixelServer {
public:
    PixelServer();
    virtual ~PixelServer();
    void run();
    void stop();
    void setPixel(Uint32 x, Uint32 y, Uint32 c);

private:
    UIWindow window;
    Net *server;
    
    UILayer pxLayer;
    UILayer guiLayer;    
};

#endif	/* PIXELSERVER_H */

