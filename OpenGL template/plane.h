#ifndef plane_h
#define plane_h

#include "GL/glew.h"
#include "loadobj.h"
#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "LoadTGA.h"
#include "math.h"

mat4 placingPlane(vec3 l, vec3 p, vec3 s, vec3 v);
void yawTurn(bool isTurnLeft, bool isTurnRight);
void pitchTurn(bool isUp, bool isDown);

#endif