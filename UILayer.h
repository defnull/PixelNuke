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
    void setPx(unsigned int x, unsigned int y, unsigned int c);
    unsigned int getPx(unsigned int x, unsigned int y);
    void saveAs(const char * filename);
    const GLuint texSize;
private:
    size_t texMem;
    GLenum texFormat;
    GLuint texId = 0;
    GLuint texPBO1 = 0;
    GLuint texPBO2 = 0;
    GLubyte *texData = NULL;
    std::mutex resizeMutex;
};

#endif	/* UILAYER_H */

