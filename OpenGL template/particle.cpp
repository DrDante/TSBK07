#include "particle.h"

particle* GenerateParticles(int nrOfParticles, int isExplosion)
{
	particle *particleArray = (particle *)malloc(sizeof(particle)*nrOfParticles);

	GLfloat xSpeed;
	GLfloat ySpeed;
	GLfloat zSpeed;

	GLfloat xAcc;
	GLfloat yAcc;
	GLfloat zAcc;
	vec3 speed = {0.07f, 0.07f, 0.07f};
	vec3 tempSpeed = { 0.5f, 0.5f, 0.5f };
	int scale = 40000;
	for (int i = 0; i < nrOfParticles; i++)
	{
		xAcc = 0;
		yAcc = -0.01;
		zAcc = 0;
		if (isExplosion)
		{
			speed.x = speed.x + (GLfloat)rand() / 15000;
			speed.y = speed.y + (GLfloat)rand() / 15000;
			speed.z = speed.z + (GLfloat)rand() / 15000;
		}
		else
		{
			speed.x = speed.x + (GLfloat)rand() / 150000;
			speed.y = speed.y + (GLfloat)rand() / 80000;
			speed.z = speed.z + (GLfloat)rand() / 150000;
		}

		tempSpeed = MultVec3(Ry(2 * 3.14*((GLfloat)i / (GLfloat)nrOfParticles)), speed);

		speed = { 0.07f, 0.07f, 0.07f };

		particleArray[i].SetXAcc(xAcc);
		particleArray[i].SetYAcc(yAcc);
		particleArray[i].SetZAcc(zAcc);

		particleArray[i].SetXSpeed(tempSpeed.x);
		particleArray[i].SetYSpeed(tempSpeed.y);
		particleArray[i].SetZSpeed(tempSpeed.z);
	}
	return particleArray;
}