#include "GL/glew.h"
#include "loadobj.h"
#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "LoadTGA.h"
#include "math.h"
#include "terrain.h"

// Type 1 is EU55_1
class tree{
public: 
	vec3 pos;
	GLfloat size;
	GLfloat hitBoxHeigth;
	GLfloat hitBoxRad;
	int type;

private:
	void GenerateHitBox();
};