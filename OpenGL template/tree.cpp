#include "tree.h"

// Absolutely not working
bool tree::CheckHitBox(vec3 objectPos){
	GLfloat R = 0.001;
	GLfloat gg;
	switch (type)
	{
	case 1:		
		gg = sqrt(pow((pos.x - objectPos.x), 2) + pow((pos.z - objectPos.z), 2));
		if (gg < R)
		{
			return true;
		}
		break;
	case 2:
		// Do something
		break;
	case 3:
		// Do something
		break;
	}
}