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

int getWidth(TextureData *tex);

int getHeight(TextureData *tex);

// Find the height of the terrain, given a x- and z-coordinate
GLfloat findHeight(GLfloat x, GLfloat z, GLfloat *vertexArray, int width, int height); 

#endif