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

class PixelServer : NonCopyable {
public:
    PixelServer();
    ~PixelServer();
    void run();
    void stop();
    void setPixel(Uint32 x, Uint32 y, Uint32 c);

private:
    Net server;
    UIWindow window;
    UILayer pxLayer;
    UILayer guiLayer;    
};

#endif	/* PIXELSERVER_H */

