/* 
 * File:   UIWindow.h
 * Author: marc
 *
 * Created on March 20, 2015, 3:20 PM
 */

#ifndef UIWINDOW_H
#define	UIWINDOW_H
#include "UILayer.h"
#include <vector>
#include <stdint.h>
#include <GLFW/glfw3.h>

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
    GLFWwindow* window;
    std::vector<UILayer*> layers;
    Uint32 maxfps = 30;
    unsigned int frameCounter = 0;

};

#endif	/* UIWINDOW_H */

