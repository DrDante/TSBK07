#ifndef plane_h
#define plane_h

#include "GL/glew.h"
#include "loadobj.h"
#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "LoadTGA.h"
#include "math.h"

void initPlane();
mat4 placingPlane(vec3 l, vec3 p, vec3 s, vec3 v);
void planeSideTurn(bool isTurnLeft, bool isTurnRight);
bool planeReadyToTurn();
void pitchTurn(bool isUp, bool isDown);

#endif