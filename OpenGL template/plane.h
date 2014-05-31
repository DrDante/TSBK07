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
	vec3 dir = { 1.0, 0.0, 0.0 };	// Should always be normalized.
	vec3 up = { 0.0, 1.0, 0.0 };	// Should always be normalized.
	const float lowestVel = 0.2;	// The lowest velocity where the plane won't fall.
	float vel = 0.0;
	bool isCollided = FALSE;
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
		if (!isCollided){
			dir = direction;
			up = upVector;
		}
	}
	float GetVelocity()
	{
		return vel;
	}
	void SetVelocity(float velocity)
	{
		vel = velocity;
	}
	void SetCollision(bool isCol)
	{
		isCollided = isCol;
	}
	void SetPosition(vec3 position)
	{
		pos = position;
	}
	void MovePlane()
	{
		if (!isCollided)
		{
			pos += dir * vel;

			vec3 tempRight = Normalize(CrossProduct(dir, up));
			if (tempRight.y != 0.0)
			{
				dir = Normalize(dir - 0.015 * DotProduct(tempRight, vec3(0.0, 1.0, 0.0)) * tempRight); // Turning.
				// 0.015 above could use some fine tuning.
			}
		}
		if (vel < lowestVel)
		{
			pos -= vec3(0.0, (lowestVel - vel), 0.0) * 1.5;
		}
	}
};

#endif