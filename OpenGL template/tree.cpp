#include "tree.h"

// Absolutely not working
bool tree::CheckHitBox(vec3 objectPos){
	GLfloat R = 0.001;
	GLfloat distanceToObj;
	bool isHit=FALSE;
	switch (type)
	{
	case 1:		
		distanceToObj = sqrt(pow((pos.x - objectPos.x), 2) + pow((pos.z - objectPos.z), 2));
		if (distanceToObj < R)
		{
			isHit=TRUE;
		}
		break;
	case 2:
		// Do something
		break;
	case 3:
		// Do something
		break;
	}

	return isHit;
}