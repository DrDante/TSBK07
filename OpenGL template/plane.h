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

class Plane{
	vec3 pos = { 0, 0, 0 };
	vec3 dir = { 1, 0, 0 }; // Should always be normalized.
	vec3 up = { 0, 1, 0 }; // Should always be normalized.
	float vel = 0.0;
public:
	Plane(vec3 position, vec3 direction, float velocity)
	{
		pos = position;
		dir = direction;
		vel = velocity;
	}
	vec3 GetPosition()
	{
		return pos;
	}
	vec3 GetDirection()
	{
		return dir;
	}
	vec3 GetUpVector()
	{
		return up;
	}
	void SetDirection(vec3 direction, vec3 upVector)
	{
		dir = direction;
		up = upVector;
	}
	float GetVelocity()
	{
		return vel;
	}
	void SetVelocity(float velocity)
	{
		vel = velocity;
	}
	void MovePlane()
	{
		pos += dir * vel;
	}
};

#endif