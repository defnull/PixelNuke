/* 
 * File:   PixelServer.cpp
 * Author: marc
 * 
 * Created on March 22, 2015, 2:48 PM
 */

#include "PixelServer.h"
#include "Net.h"

PixelServer::PixelServer() :
server(),
window(),
pxLayer(1024, 1024, false),
guiLayer(1024, 1024, true) {
    window.addLayer(&pxLayer);
    window.addLayer(&guiLayer);
    server.setCallback("PX",
        [&](const NetSession &s, const std::string &line) {
            s.send("PX "+line);
        });
    server.watch(8080);
    
    std::thread netThread ([&] {server.loop();});
    netThread.detach();
}

void PixelServer::setPixel(Uint32 x, Uint32 y, Uint32 c) {
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

