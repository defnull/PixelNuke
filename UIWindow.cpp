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

UIWindow::UIWindow() {

	glfwSetErrorCallback([](int code, const char * msg){
		printf("GLFW Error: %d %s", code, msg);
	});

    if (!glfwInit())
        throw std::runtime_error("EXIT: Could not initiate GLFW");

    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    window = glfwCreateWindow(640, 480, "Pixelflut", NULL, NULL);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("EXIT: Could not create OpenGL context and/or window");
    }

	glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, (void*) this);
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

}

void UIWindow::addLayer(UILayer *layer) {
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    layer->resize(w, h);
    layers.push_back(layer);
}

UIWindow::~UIWindow() {
    glfwTerminate();
}

void UIWindow::loop() {
    bool done = false;

	/* Make the window's context current */
	glfwSwapInterval(1);
	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		UIWindow *win = static_cast<UIWindow*>(glfwGetWindowUserPointer(window));
		printf("KEY: %x %x %x %x\n", key, scancode, action, mods);
	});

	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int w, int h) {
		UIWindow *win = static_cast<UIWindow*>(glfwGetWindowUserPointer(window));
	    for(UILayer* layer: win->layers) {
	        layer->resize(w, h);
	    };
	});


	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window) && !done) {

		auto preSwap = glfwGetTime();
		glfwSwapBuffers(window);

		auto preDraw = glfwGetTime();
		draw();

		auto preEvents = glfwGetTime();
		glfwPollEvents();

		auto preWait = glfwGetTime();

		if(frameCounter++ % 100 == 0)
			printf("Draw: %02.0f%% Event: %02.0f%%\n",
					(preEvents-preDraw)/(1.0f/maxfps)*100,
					(preWait-preEvents)/(1.0f/maxfps)*100);

		int wait = ((1.0f/maxfps)-(preWait-preSwap))*1000;
		if(wait > 10)
			std::this_thread::sleep_for(std::chrono::milliseconds(wait));
	}

    glfwTerminate();
}

void UIWindow::stop() {
	glfwSetWindowShouldClose(window, GL_TRUE);
}


void UIWindow::draw() {
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    printf("%u %u\n", w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, h, 0, -1, 1);
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(1, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    for(UILayer* layer: layers) {
        layer->draw();
    }    
}
