#ifndef cloud_h
#define cloud_h

#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "math.h"

// Type 1 is EU55_1.
class cloud{
	vec3 pos{ 0.0, 0.0, 0.0 };
	GLfloat size = 0;
	//GLfloat hitBoxHeigth = 0;
	//GLfloat hitBoxRad = 0;
	int type = 1;
public:
	cloud(vec3 pos, GLfloat size)
	{
		pos = pos;
		size = size;
	}
	vec3 GetPosition()
	{
		return pos;
	}
	GLfloat GetSize()
	{
		return size;
	}
	/*GLfloat GetHitBoxHeigth()
	{
		return hitBoxHeigth;
	}
	GLfloat GetHitBoxRad()
	{
		return hitBoxRad;
	}*/
	int GetType()
	{
		return type;
	}
	void SetXPos(GLfloat x)
	{
		pos.x = x;
	}
	void SetYPos(GLfloat y)
	{
		pos.y = y;
	}
	void SetZPos(GLfloat z)
	{
		pos.z = z;
	}
	void SetSize(GLfloat sizeIn)
	{
		size = sizeIn;
	}
	void SetType(int typeIn)
	{
		type = typeIn;
	}
};

#endif