#include "GL/glew.h"
#include "loadobj.h"
#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "LoadTGA.h"
#include "math.h"
#include "terrain.h"

int* GenerateGridPositions(GLfloat *vertexArray, int xMax, int zMax, int square_size);
int getNrXPoints();
int getNrZPoints();
GLfloat* getSizeArray();
int* getTypeArray();