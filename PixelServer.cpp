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

PixelServer::PixelServer() :
window(),
pxLayer(1024, false),
server() {
    window.addLayer(&pxLayer);

    server.setCallback("PX",
        [&](PxCommand &cmd) {
            if(cmd.nargs() == 3) {
            	GLuint x, y;
            	cmd.parse(1, x);
            	cmd.parse(2, y);

                char msg[64];
                int cx = snprintf(msg, 64, "PX %d %d %08x", x, y, pxLayer.getPx(x,y));
                if(cx >= 0 && cx < 64)
                    cmd.getClient().send(msg);
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
                pxLayer.setPx(x, y, c);
            }
        });

    server.setCallback("SIZE",
        [&](PxCommand &cmd) {
    		if(cmd.nargs() > 1) {
    			throw PxParseError("SIZE does not take any parameters.");
    		}
    		std::ostringstream oss;
    		oss << "SIZE " << window.width << " " << window.height;
    		cmd.getClient().send(oss.str());
    });

    server.watch(1234);
    
    std::thread networkThread ([&] {
    	server.loop();
    });
    networkThread.detach();

}

void PixelServer::run() {
	window.setFullscreen(true);
    window.loop();
}

void PixelServer::stop() {
    server.stop();
    window.stop();
}

PixelServer::~PixelServer() {
}

