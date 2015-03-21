/* 
 * File:   UIWindow.h
 * Author: marc
 *
 * Created on March 20, 2015, 3:20 PM
 */

#ifndef UIWINDOW_H
#define	UIWINDOW_H
#include <SDL2/SDL.h>
#include "UILayer.h"

class UIWindow {
public:
    UIWindow();
    UIWindow(const UIWindow& orig);
    virtual ~UIWindow();
    void loop();
    void draw();
private:
    SDL_Window* window;
    UILayer *pxLayer;
    UILayer *metaLayer;
    UILayer *guiLayer;
};

#endif	/* UIWINDOW_H */

