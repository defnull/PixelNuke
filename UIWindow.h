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
#include <mutex>
#include "utils.h"

class UIWindow : NonCopyable
{
public:
    UIWindow();
    virtual ~UIWindow();
    void loop();
    void draw();
    void stop();
    void addLayer(UILayer*);
    void toggleFullscreen();
    void setFullscreen(bool);
    int width=1124;
    int height=480;
private:
    void setupWindow(bool fsmode, size_t monid);
    void onResize();
    bool fullscreen = false;
    bool monitor_id = 0;
    std::mutex window_mutex;
    std::mutex draw_mutex;
    GLFWwindow* window = NULL;
    std::vector<UILayer*> layers;
    Uint32 maxfps = 30;
    unsigned int frameCounter = 0;
};

#endif	/* UIWINDOW_H */

