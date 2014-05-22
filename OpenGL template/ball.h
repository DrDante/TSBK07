#ifndef ball_h
#define ball_h

#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "math.h"


class ball{

	vec3 pos = { 0.0f, 0.0f, 0.0f };
	GLfloat color = 0;
	bool alreadyHit = false;
	//GLfloat hitBoxHeigth = 0;
	//GLfloat hitBoxRad = 0;
	int type = 1;
public:
	ball(vec3 pos, GLfloat size)
	{
		pos = pos;
	}
	vec3 GetPosition()
	{
		return pos;
	}
	bool CheckHitBox(vec3 objectPos){
		GLfloat R = 3;
		GLfloat dist;
		bool isHit = false;
		dist = sqrt(pow((pos.x - objectPos.x), 2) + pow((pos.y - objectPos.y), 2) +  pow((pos.z - objectPos.z), 2));
			if (dist < R)
			{
				isHit = true;
				alreadyHit = false;
			}

		return isHit;
	}
	bool CheckOtherObject(vec3 objectPos){
		GLfloat R = 7;
		GLfloat dist;
		bool inOtherObject = false;
		dist = sqrt(pow((pos.x - objectPos.x), 2) + pow((pos.y - objectPos.y), 2) + pow((pos.z - objectPos.z), 2));
		if (dist < R)
		{
			inOtherObject = true;
		}
		return inOtherObject;
	}
	bool AlreadyHit()
	{
		return alreadyHit;
	}
	void ChangePosition(vec3 deltaPos)
	{
		pos.x = pos.x + deltaPos.x;
		pos.y = pos.y + deltaPos.y;
		pos.z = pos.z + deltaPos.z;
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
};

#endif