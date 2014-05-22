#ifndef terrain_h
#define terrain_h

#include "GL/glew.h"
#include "loadobj.h"
#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "LoadTGA.h"
#include "math.h"
#include "tree.h"
#include "GenerateGridPositions.h"
#include "cloud.h"



Model* GenerateTerrain(TextureData *tex, GLfloat terrainScale);

vec3 giveNormal(int x, int y, int z, GLfloat *vertexArray, GLuint *indexArray, int width, int height);

int getWidth(TextureData *tex);

int getHeight(TextureData *tex);

tree* GetForest(GLfloat *vertexArray, int xMax, int zMax, int square_size);
cloud* GetClouds(GLfloat *vertexArray, int xMax, int zMax, int square_size);
int GetNrOfTrees();
int GetNrOfClouds();

// Find the height of the terrain, given a x- and z-coordinate
GLfloat findHeight(GLfloat x, GLfloat z, GLfloat *vertexArray, int width, int height); 

#endif