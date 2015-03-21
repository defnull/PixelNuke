/* 
 * File:   UILayer.cpp
 * Author: marc
 * 
 * Created on March 21, 2015, 8:26 PM
 */

#include "UILayer.h"
#include <cstring>
#include <cstdio>

UILayer::UILayer(GLuint width, GLuint height, bool alpha) :
width(width), height(height) {
    texWidth = 64;
    while (texWidth < width || texWidth < height)
        texWidth = texWidth * 2;
    texHeight = texWidth;
    texFormat = alpha ? GL_RGBA8 : GL_RGB;
    allocate();
}

void UILayer::allocate() {
    if (texData != NULL)
        cleanup();

    auto nbytes = getTexSize();

    texData = new GLubyte[nbytes];

    // Create texture object
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, texFormat, texWidth, texHeight, 0, texFormat, GL_UNSIGNED_BYTE, NULL);
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

void UILayer::cleanup() {
    if (texData != NULL) {
        glDeleteTextures(1, &texId);
        glDeleteBuffers(1, &texPBO1);
        glDeleteBuffers(1, &texPBO2);
        delete texData;
    }
}

void UILayer::draw() {
    if (texData != NULL) return;

    GLuint pboNext = texPBO1;
    GLuint pboIndex = texPBO2;
    texPBO1 = pboIndex;
    texPBO2 = pboNext;

    // Switch PBOs on each call. One is updated, one is drawn.

    // Update texture from first PBO
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIndex);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, texFormat, texWidth, texHeight, 0, texFormat, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    // Update second PBO with new pixel data
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboNext);
    GLubyte *ptr = (GLubyte*) glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
    memcpy(ptr, texData, getTexSize());
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    // Release PBOs
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    //// Actually draw stuff. The texture should be updated in the meantime.

    int quadsize;
    if (width > texWidth || height > texHeight)
        quadsize = (width > height) ? width : height;
    else
        quadsize = texWidth;

    if (hasAlpha()) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);
    } else {
        glDisable(GL_BLEND);
    }

    glPushMatrix();
    glTranslatef(0, -height, 0); // Align quad top left instead of bottom.
    glBindTexture(GL_TEXTURE_2D, texId);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(quadsize, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(quadsize, quadsize, 0.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(0.0f, quadsize, 0.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();
}

UILayer::~UILayer() {
    cleanup();
}

size_t UILayer::getTexSize() {
    return texWidth * texHeight * (texFormat == GL_RGBA8 ? 4 : 3);
}

bool UILayer::hasAlpha() {
    return texFormat == GL_RGBA8;
}

