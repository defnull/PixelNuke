/* 
 * File:   UILayer.h
 * Author: marc
 *
 * Created on March 21, 2015, 8:26 PM
 */

#ifndef UILAYER_H
#define	UILAYER_H
#include <GL/glew.h>

class UILayer {
public:
    UILayer(GLuint width, GLuint height, bool alpha);
    ~UILayer();
    size_t getTexSize();
    bool hasAlpha();
    void draw();
    void allocate();
    void cleanup();
private:
    GLuint width, height;
    GLuint texWidth;
    GLuint texHeight;
    GLenum texFormat;
    GLuint texId;
    GLuint texPBO1;
    GLuint texPBO2;
    GLubyte *texData = NULL;
};

#endif	/* UILAYER_H */

