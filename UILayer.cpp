/* 
 * File:   UILayer.cpp
 * Author: marc
 * 
 * Created on March 21, 2015, 8:26 PM
 */

#include "UILayer.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <algorithm>


UILayer::UILayer(unsigned int width, unsigned int height, bool alpha) :
width(width), height(height) {
	texSize = 64;
    while (texSize < width || texSize < height)
    	texSize *= 2;
    texFormat = alpha ? GL_RGBA8 : GL_RGB;
    
    auto nbytes = getTexSize();
    texData = new GLubyte[nbytes];

    // Create texture object
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Create two PBOs
    glGenBuffers(1, &texPBO1);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texPBO1);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, nbytes, NULL, GL_STREAM_DRAW);
    glGenBuffers(1, &texPBO2);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texPBO2);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, nbytes, NULL, GL_STREAM_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void UILayer::resize(unsigned int w, unsigned int h) {
    std::lock_guard<std::mutex> lock(resizeMutex);

	unsigned int tSize = 64;

	width = w;
	height = h;

    while (tSize < w || tSize < h)
    	tSize *= 2;

    // Never make it smaller. This way we can make setPx lock-free
    if(tSize <= texSize) {
    	return;
    }

    auto nbytes = tSize * tSize * (texFormat == GL_RGBA8 ? 4 : 3);
    GLubyte* tData = new GLubyte[nbytes];

    // Now we copy the old tex buffer to the new image...
    unsigned int rlen = std::min(tSize, texSize) * (texFormat == GL_RGBA8 ? 4 : 3);
    for(unsigned int row = 0; row < texSize; row++) {
    	//memcpy(tData + (row*tSize), texData+(row*texSize), rlen);
    }

    // Resize PBOs
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texPBO1);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, nbytes, NULL, GL_STREAM_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texPBO2);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, nbytes, NULL, GL_STREAM_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    // Swap everything
    texData = tData;
	texSize = tSize;
}


void UILayer::setPx(unsigned int x, unsigned int y, unsigned int rgba) {
    if(x >= width || y >= height)
        return;

    pxCounter++;

    GLubyte* ptr = texData + ((y * texSize) + x) * (texFormat == GL_RGBA8 ? 4 : 3);
    GLubyte r = (rgba & 0xff000000) >> 24;
    GLubyte g = (rgba & 0x00ff0000) >> 16;
    GLubyte b = (rgba & 0x0000ff00) >> 8;
    GLubyte a = (rgba & 0x000000ff) >> 0;

    if(a == 0)
    	return;
    if (a < 0xff) {
    	GLuint na = 0xff-a;
    	r = (a * r + na * (ptr[0])) / 0xff;
    	g = (a * g + na * (ptr[1])) / 0xff;
    	b = (a * b + na * (ptr[2])) / 0xff;
    }
    ptr[0] = r;
    ptr[1] = g;
    ptr[2] = b;
}

void UILayer::draw() {
    std::lock_guard<std::mutex> lock(resizeMutex);

    GLuint pboNext = texPBO1;
    GLuint pboIndex = texPBO2;
    texPBO1 = pboIndex;
    texPBO2 = pboNext;

    // Switch PBOs on each call. One is updated, one is drawn.
    // Update texture from first PBO
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIndex);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, texFormat, texSize, texSize, 0, texFormat, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Update second PBO with new pixel data
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboNext);
    GLubyte *ptr = (GLubyte*) glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
    memcpy(ptr, texData, texSize * height * (texFormat == GL_RGBA8 ? 4 : 3));
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    //// Actually draw stuff. The texture should be updated in the meantime.

    if (hasAlpha()) {
        return;
        glEnable(GL_BLEND);
        glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);
    } else {
        glDisable(GL_BLEND);
    }

    float tw = width / (float) texSize;
    float th = height / (float) texSize;

    glPushMatrix();
    //glTranslatef(0, height, 0); // Align quad top left instead of bottom.
    glBindTexture(GL_TEXTURE_2D, texId);
    glBegin(GL_QUADS);
    	glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, tw);   glVertex3f(0.0f, width, 0.0f);
        glTexCoord2f(th, tw);     glVertex3f(height, width, 0.0f);
        glTexCoord2f(th, 0.0f);   glVertex3f(height, 0.0f, 0.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();
}

UILayer::~UILayer() {
    glDeleteTextures(1, &texId);
    glDeleteBuffers(1, &texPBO1);
    glDeleteBuffers(1, &texPBO2);
    delete[] texData;
}

size_t UILayer::getTexSize() {
    return texSize * texSize * (texFormat == GL_RGBA8 ? 4 : 3);
}

bool UILayer::hasAlpha() {
    return texFormat == GL_RGBA8;
}

