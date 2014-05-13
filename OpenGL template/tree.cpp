#include "tree.h"

void tree::GenerateHitBox(){
	switch (type)
	{
	case 1:
		hitBoxRad = sqrt(pow((pos.x - 0.2), 2) + pow((pos.z - 0.2), 2));
		hitBoxHeigth = 1 + pos.y;
		break;
	case 2:
		// Do something
		break;
	case 3:
		// Do something
		break;
	}
}