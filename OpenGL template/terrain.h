#ifndef terrain_h
#define terrain_h

#include "GL/glew.h"
#include "loadobj.h"
#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "LoadTGA.h"
#include "math.h"

Model* GenerateTerrain(TextureData *tex);

vec3 giveNormal(int x, int y, int z, GLfloat *vertexArray, GLuint *indexArray, int width, int height);

int getWidth(TextureData *tex); //Only needed locally?

int getHeight(TextureData *tex);

#endif