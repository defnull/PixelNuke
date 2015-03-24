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

class UILayer : NonCopyable {
public:
    UILayer(GLuint width, GLuint height, bool alpha);
    ~UILayer();
    size_t getTexSize();
    bool hasAlpha();
    void draw();
    void allocate();
    void cleanup();
    void setPx(unsigned int x, unsigned int y, unsigned int c);
private:
    GLuint width, height;
    GLuint texWidth;
    GLuint texHeight;
    GLenum texFormat;
    GLuint texId = 0;
    GLuint texPBO1 = 0;
    GLuint texPBO2 = 0;
    GLubyte *texData = NULL;
};

#endif	/* UILAYER_H */

