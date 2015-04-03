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
    	gettimeofday(&connected, NULL);
    };

    unsigned int pixel = 0;
    timeval connected;
};


PixelServer::PixelServer() :
window(),
pxLayer(1024, false),
guiLayer(1024, true),
server() {
    window.addLayer(&pxLayer);
    window.addLayer(&guiLayer);

    server.setSessionCallbacks(
    		[&](NetSession *s) {
    	s->data = new PixelSession();
    },
			[&](NetSession *s) {
		auto pxs = static_cast<PixelSession*>(s->data);
		printf("Disconnect after %u\n", pxs->pixel);
    	delete pxs;
    });

    server.setCallback("PX",
        [&](PxCommand &cmd) {
    		auto pxs = static_cast<PixelSession*>(cmd.getClient().data);
    		pxs->pixel++;
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

    server.setCallback("HELP",
        [&](PxCommand &cmd) {
            cmd.getClient().send(
            "HELP: == PIXELFLUT ==\n"
            "HELP: Line based ASCII protocol\n"
            "HELP: Commands:\n"
            "HELP:   PX <x> <y> <RRGGBB>\n"
            "HELP:   PX <x> <y>\n"
            "HELP:   SIZE\n"
            "HELP: and more ;)");
    });

    server.setCallback("STAT",
        [&](PxCommand &cmd) {
    		for(auto const & sess: cmd.getClient().getServer().getSessions()) {
        		auto pxs = static_cast<PixelSession*>(cmd.getClient().data);
    			auto addr = ((sockaddr_in6*) &(sess->addr))->sin6_addr;
    		    char ip[50];
    		    inet_ntop(PF_INET6, (struct in_addr6*)&(addr), ip, sizeof(ip)-1);
    			char msg[128];
    			snprintf(msg, 128, "STAT %s %u", ip, pxs->pixel);
    			cmd.getClient().send(msg);
    		}
    });

    server.watch(1234);
    
    std::thread networkThread ([&] {
    	server.loop();
    });
    networkThread.detach();

    running = true;

    std::thread screenshotThread ([&] {
    	char filename[128];
		struct timeval tp;
    	while(running) {
    		gettimeofday(&tp, NULL);
    		long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000; //get current timestamp in milliseconds
    		snprintf(filename, 128, "log/px_%lu.png", ms);
    		pxLayer.saveAs(filename);
    		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    	}
    });
    screenshotThread.detach();


}

void PixelServer::run() {
	window.setFullscreen(true);
    window.loop();
}

void PixelServer::stop() {
    running = false;
    server.stop();
    window.stop();
}

PixelServer::~PixelServer() {
}

