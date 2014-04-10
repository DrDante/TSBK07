#ifndef terrain_h
#define terrain_h

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#elif defined(_WIN32)
#include "GL/glew.h"
#else
#include <GL/gl.h>
#endif

Model* GenerateTerrain(TextureData *tex);

vec3 giveNormal(int x, int y, int z, GLfloat *vertexArray, GLuint *indexArray, int width, int height);

int getWidth(TextureData *tex); //Only needed locally?

int getHeight(TextureData *tex);

#endif