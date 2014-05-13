#ifndef plane_h
#define plane_h

#include "GL/glew.h"
#include "loadobj.h"
#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "LoadTGA.h"
#include "math.h"

//void initPlane();
//mat4 placingPlane(vec3 l, vec3 p, vec3 s, vec3 v);
//void planeSideTurn(bool isTurnLeft, bool isTurnRight);
//bool planeReadyToTurn();
//void pitchTurn(bool isUp, bool isDown);

class Plane{
	vec3 pos = { 0.0, 0.0, 0.0 };
	vec3 dir = { 1.0, 0.0, 0.0 }; // Should always be normalized.
	vec3 up = { 0.0, 1.0, 0.0 }; // Should always be normalized.
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
		//pos.y -= 0.1; // Gravity.
		//pos.y += 0.1 * DotProduct(up, vec3(0.0, 1.0, 0.0)); // Lift force.
		//pos += vec3(up.x, 0, up.z); // "Turning" force.
		vec3 tempRight = Normalize(CrossProduct(dir, up));
		if (tempRight.y < 0.0)
		{
			dir = Normalize(dir + 0.015 * DotProduct(tempRight, vec3(0.0, -1.0, 0.0)) * tempRight); // Turn right.
			// 0.01 above could use some fine tuning.
		}
		else if (tempRight.y > 0.0)
		{
			dir = Normalize(dir - 0.015 * DotProduct(tempRight, vec3(0.0, 1.0, 0.0)) * tempRight); // Turn left.
			// 0.01 above could use some fine tuning.
		}
	}
};

#endif