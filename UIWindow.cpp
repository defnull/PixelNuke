/* 
 * File:   UIWindow.cpp
 * Author: marc
 * 
 * Created on March 20, 2015, 3:20 PM
 */

#include "UIWindow.h"
#include "UILayer.h"
#include <GL/glew.h>

UIWindow::UIWindow() {
    SDL_SetMainReady();
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_DisableScreenSaver();

    window = SDL_CreateWindow(
            "Pixelflut",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            640, 480,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE); //|SDL_WINDOW_BORDERLESS);

    SDL_GL_CreateContext(window);

    glewInit();

    //glShadeModel(GL_FLAT);            // shading mathod: GL_SMOOTH or GL_FLAT
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // 4-byte pixel alignment
    //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);    
}

void UIWindow::addLayer(UILayer *layer) {
    layers.push_back(layer);
}


UIWindow::UIWindow(const UIWindow& orig) {
}

UIWindow::~UIWindow() {
    

}

void UIWindow::loop() {
    SDL_Event event;
    bool done = false;

    while (!done) {
        Uint32 ticks = SDL_GetTicks();
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_USEREVENT:
                    break;
                case SDL_KEYDOWN:
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    break;
                case SDL_QUIT:
                    done = true;
                    break;
                default:
                    break;
            }
        }
        draw();
        SDL_GL_SwapWindow(window);
        Uint32 dt = SDL_GetTicks() - ticks;
        if(dt < 1000/maxfps) {
            SDL_Delay(1000/maxfps - dt);
        }
    }
}

void UIWindow::stop() {
    SDL_Event sdlevent;
    sdlevent.type = SDL_QUIT;
    SDL_PushEvent(&sdlevent);
}


void UIWindow::draw() {
    int w, h;
    SDL_GetWindowSize(window, &w, &h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, 0, h, -1, 1);
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    for(UILayer* layer: layers) {
        layer->draw();
    }    
}
