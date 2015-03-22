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
#include <vector>

class UIWindow {
public:
    UIWindow();
    UIWindow(const UIWindow& orig);
    virtual ~UIWindow();
    void loop();
    void draw();
    void stop();
    void addLayer(UILayer*);
private:
    SDL_Window* window;
    std::vector<UILayer*> layers;
    Uint32 maxfps = 30;
};

#endif	/* UIWINDOW_H */

