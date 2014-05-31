#ifndef tree_h
#define tree_h

#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "math.h"

// Type 1 is EU55_1.
class tree{
	vec3 pos{ 0.0, 0.0, 0.0};
	GLfloat size = 0;
	int type = 1;
public: 
	tree(vec3 pos, GLfloat size)
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
		
	bool CheckHitBox(vec3 objectPos)
	{
		GLfloat R = 3;
		GLfloat height = pos.y + 0.8*size;
		GLfloat dist;
		bool isHit = false;
		switch (type)
		{
		case 1:
			dist = sqrt(pow((pos.x - objectPos.x), 2) + pow((pos.z - objectPos.z), 2));
			if (dist < R && objectPos.y < height)
			{
				isHit = true;
			}
			break;
		case 2:
			dist = sqrt(pow((pos.x - objectPos.x), 2) + pow((pos.z - objectPos.z), 2));
			if (dist < R && objectPos.y < height)
			{
				isHit = true;
			}
			break;
		case 3:
			// Reserved.
			break;
		}
		return isHit;
	}
};

#endif