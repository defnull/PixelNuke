/* 
 * File:   PixelServer.cpp
 * Author: marc
 * 
 * Created on March 22, 2015, 2:48 PM
 */

#include "PixelServer.h"
#include "Net.h"
#include <thread>

PixelServer::PixelServer() :
window(),
pxLayer(100, 100, false),
guiLayer(100, 100, true),
server() {
    window.addLayer(&pxLayer);
    window.addLayer(&guiLayer);

    server.setCallback("PX",
        [&](PxCommand &cmd) {
            if(cmd.nargs() == 3) {
            	GLuint x, y;
            	cmd.parse(1, x);
            	cmd.parse(2, y);
            } else if (cmd.nargs() == 4) {
            	GLuint x,y,c;
            	cmd.parse(1, x);
            	cmd.parse(2, y);
            	if(cmd.len(3) == 6) {
                	cmd.parse(3, c, 16);
                	c = (c<<8) + 0xff;
            	} else if (cmd.len(3) == 8) {
                	cmd.parse(3, c, 16);
            	} else {
            		throw PxParseError("Color must have 3 or 4 byte components.");
            	}
                setPixel(x,y,c);
            }
        });
    
    std::thread networkThread ([&] {
    	server.loop();
    });
    networkThread.detach();

    server.watch(8080);
}

void PixelServer::setPixel(unsigned int x, unsigned int y, unsigned int c) {
    pxLayer.setPx(x, y, c);
}

void PixelServer::run() {
    window.loop();
}

void PixelServer::stop() {
    server.stop();
    window.stop();
}

PixelServer::~PixelServer() {
}

