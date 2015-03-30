/* 
 * File:   UILayer.h
 * Author: marc
 *
 * Created on March 21, 2015, 8:26 PM
 */

#ifndef UILAYER_H
#define	UILAYER_H
#include <GL/glew.h>
#include "utils.h"
#include <mutex>

class UILayer : NonCopyable {
public:
    UILayer(unsigned int texSize, bool alpha);
    ~UILayer();
    bool hasAlpha();
    void draw();
    void allocate();
    void cleanup();
    void setPx(unsigned int x, unsigned int y, unsigned int c);
private:
    GLuint texSize;
    size_t texMem;
    GLenum texFormat;
    GLuint texId = 0;
    GLuint texPBO1 = 0;
    GLuint texPBO2 = 0;
    GLubyte *texData = NULL;
    std::mutex resizeMutex;
    unsigned int pxCounter = 0;
};

#endif	/* UILAYER_H */

