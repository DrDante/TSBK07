#ifndef plane_h
#define plane_h

#include "GL/glew.h"
#include "loadobj.h"
#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "LoadTGA.h"
#include "math.h"

class Plane{
	vec3 pos = { 0.0, 0.0, 0.0 };
	vec3 dir = { 1.0, 0.0, 0.0 }; // Should always be normalized.
	vec3 up = { 0.0, 1.0, 0.0 }; // Should always be normalized.
	float vel = 0.0;
	bool isCollided=FALSE; // TRUE if collision, FALSE if not (Right now: TRUE if pos.y under ground height, FALSE otherwise)
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
	void SetCollision(bool isCol){ // A func that can be used to change isCollided to TRUE/ FALSE. isCollided needs to be handled futher
		isCollided = isCol;
	}
	void MovePlane()
	{
		pos += dir * vel;
		//pos.y -= 0.1; // Gravity.
		//pos.y += 0.1 * DotProduct(up, vec3(0.0, 1.0, 0.0)); // Lift force.

		// OBS! Subjektivt, bör diskuteras.
		vec3 tempRight = Normalize(CrossProduct(dir, up));
		if (tempRight.y != 0.0)
		{
			dir = Normalize(dir - 0.015 * DotProduct(tempRight, vec3(0.0, 1.0, 0.0)) * tempRight); // Turning.
			// 0.015 above could use some fine tuning.
		}
	}
};

#endif