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
#include "dep/lodepng/lodepng.h"

UILayer::UILayer(unsigned int texSize, bool alpha) :
		texSize(texSize), texFormat(alpha ? GL_RGBA : GL_RGB) {

	texMem = texSize * texSize * (texFormat == GL_RGBA ? 4 : 3);
	texData = new GLubyte[texMem];

	// Create texture object
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Create two PBOs
	glGenBuffers(1, &texPBO1);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texPBO1);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, texMem, NULL, GL_STREAM_DRAW);
	glGenBuffers(1, &texPBO2);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texPBO2);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, texMem, NULL, GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void UILayer::setPx(unsigned int x, unsigned int y, unsigned int rgba) {
	if (x >= texSize || y >= texSize)
		return;

	GLubyte* ptr = texData
                 + ((y * texSize) + x) * (texFormat == GL_RGBA ? 4 : 3);
	GLubyte r = (rgba & 0xff000000) >> 24;
	GLubyte g = (rgba & 0x00ff0000) >> 16;
	GLubyte b = (rgba & 0x0000ff00) >> 8;
	GLubyte a = (rgba & 0x000000ff) >> 0;

	if(hasAlpha()) {
        ptr[0] = r;
        ptr[1] = g;
        ptr[2] = b;
        ptr[3] = a;
        return;
	}
	if (a == 0)
		return;
	if (a < 0xff) {
		GLuint na = 0xff - a;
		r = (a * r + na * (ptr[0])) / 0xff;
		g = (a * g + na * (ptr[1])) / 0xff;
		b = (a * b + na * (ptr[2])) / 0xff;
		return;
	}
	ptr[0] = r;
	ptr[1] = g;
	ptr[2] = b;
}

unsigned int UILayer::getPx(unsigned int x, unsigned int y) {
	if (x >= texSize || y >= texSize)
		return 0x00000000;

	GLubyte* ptr = texData
                 + ((y * texSize) + x) * (texFormat == GL_RGBA ? 4 : 3);

	return (ptr[0] << 24) + (ptr[1] << 16) + (ptr[2] << 8) + 0xff;
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
	glTexImage2D(GL_TEXTURE_2D, 0, texFormat, texSize, texSize, 0, texFormat,
			GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Update second PBO with new pixel data
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboNext);
	GLubyte *ptr = (GLubyte*) glMapBuffer(GL_PIXEL_UNPACK_BUFFER,
			GL_WRITE_ONLY);
	memcpy(ptr, texData, texMem);
	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	//// Actually draw stuff. The texture should be updated in the meantime.

	if (hasAlpha()) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	} else {
		glDisable(GL_BLEND);
	}

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(0.0f, 0.0f, 0.0f);
	    glTexCoord2f(0, 1); glVertex3f(0.0f, texSize, 0.0f);
	    glTexCoord2f(1, 1); glVertex3f(texSize, texSize, 0.0f);
	    glTexCoord2f(1, 0); glVertex3f(texSize, 0.0f, 0.0f);
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

bool UILayer::hasAlpha() {
	return texFormat == GL_RGBA;
}

void UILayer::saveAs(const char* filename) {
	GLubyte tmp[texMem];
	memcpy(&tmp, texData, texMem);
	if(hasAlpha()) {
		lodepng_encode32_file(filename, tmp, texSize, texSize);
	} else {
		lodepng_encode24_file(filename, tmp, texSize, texSize);
	}
}
