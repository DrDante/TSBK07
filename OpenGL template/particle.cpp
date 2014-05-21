#include "particle.h"

particle* GenerateParticles(int nrOfParticles)
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
		//Hmm.. hur räkna ut hastigheten?
		xAcc = 0;
		yAcc = -0.01;
		zAcc = 0;

		speed.x = speed.x + (GLfloat)rand() / 1300000;
		speed.y = speed.y + (GLfloat)rand() / 100000;
		speed.z = speed.z + (GLfloat)rand() / 1300000;

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