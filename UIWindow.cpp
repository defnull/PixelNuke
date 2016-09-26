/*
 * File:   UIWindow.cpp
 * Author: marc
 *
 * Created on March 20, 2015, 3:20 PM
 */

#include "UIWindow.h"
#include "UILayer.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <algorithm>      // std::max


UIWindow::UIWindow() {

	glfwSetErrorCallback([](int code, const char * msg){
		printf("GLFW Error: %d %s", code, msg);
	});

    if (!glfwInit())
        throw std::runtime_error("EXIT: Could not initiate GLFW");

    setupWindow(0, false);
}

void UIWindow::setupWindow(bool fsmode, size_t monid) {
    std::lock_guard<std::mutex> lock(window_mutex);
    auto oldWindow = window;

    glfwWindowHint(GLFW_DOUBLEBUFFER, 1);

    if(fsmode) {
        int count;
        GLFWmonitor** monitors = glfwGetMonitors(&count);

        monitor_id = monid % count;
        printf("Switching to monitor: %u (of %u)\n", monitor_id, count);
        fullscreen = true;
        GLFWmonitor* monitor = monitors[monitor_id];
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        window = glfwCreateWindow(mode->width, mode->height, "Pixelflut", monitor, oldWindow);
    } else {
        fullscreen = false;
        window = glfwCreateWindow(640, 480, "Pixelflut", NULL, oldWindow);
    }

    if (!window) {
        glfwTerminate();
        throw std::runtime_error("EXIT: Could not create OpenGL context and/or window");
    }

    glfwSetWindowUserPointer(window, (void*) this);
	glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
      throw std::runtime_error("EXIT: Glew initialization failed");
    }

    //glShadeModel(GL_FLAT);            // shading mathod: GL_SMOOTH or GL_FLAT
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // 4-byte pixel alignment
    //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
	onResize();

	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		UIWindow *win = static_cast<UIWindow*>(glfwGetWindowUserPointer(window));

		printf("KEY: %u %u %u %u\n", key, scancode, action, mods);
		if(action != GLFW_PRESS) return;

        if (key == GLFW_KEY_F11) {
            win->toggleFullscreen();
        } else if (key == GLFW_KEY_F12) {
            win->setupWindow(true, win->monitor_id + 1);
        } else if (key == GLFW_KEY_Q
                || key == GLFW_KEY_ESCAPE) {
            win->stop();
        }
	});

	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int w, int h) {
		UIWindow *win = static_cast<UIWindow*>(glfwGetWindowUserPointer(window));
		win->onResize();
	});

    if(oldWindow) {
        glfwDestroyWindow(oldWindow);
    }

}

void UIWindow::toggleFullscreen() {
    setFullscreen(!fullscreen);
}

void UIWindow::setFullscreen(bool fsmode) {
    setupWindow(fsmode, monitor_id);
}

void UIWindow::addLayer(UILayer *layer) {
    layers.push_back(layer);
}

UIWindow::~UIWindow() {
    glfwTerminate();
}

void UIWindow::onResize() {
    glfwGetFramebufferSize(window, &width, &height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glViewport(0, 0, (GLsizei) width, (GLsizei) height);
    draw();
}

void UIWindow::loop() {

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window)) {

		glfwSwapBuffers(window);

		auto preDraw = glfwGetTime();
		draw();

		glfwPollEvents();

		int wait = ((1.0f/maxfps)-(glfwGetTime()-preDraw))*1000;
		if(wait > 10)
			std::this_thread::sleep_for(std::chrono::milliseconds(wait));
	}

    glfwTerminate();
}

void UIWindow::stop() {
	glfwSetWindowShouldClose(window, GL_TRUE);
}


void UIWindow::draw() {
    std::lock_guard<std::mutex> lock(draw_mutex);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    for(UILayer* layer: layers) {
    	glPushMatrix();

        int ts = layer->texSize;
        if(width > ts || height > ts) {
            float scale = std::max(width, height) / (float) ts;
            glScalef(scale, scale, 1);
        }

        layer->draw();
    	glPopMatrix();
    }
}
