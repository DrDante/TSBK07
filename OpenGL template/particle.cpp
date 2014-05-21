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

	for (int i = 0; i < nrOfParticles; i++)
	{
		//Hmm.. hur räkna ut hastigheten?
		if (i < 5)
		{
			xSpeed = -rand()/10000;
			ySpeed = 0.5;
			zSpeed = -rand()/10000;
		}
		else
		{
			xSpeed = rand() / 10000;
			ySpeed = 0.5;
			zSpeed = rand() / 10000;
		}

		xAcc = 0;
		yAcc = -0.1;
		zAcc = 0;
		

		particleArray[i].SetXAcc(xAcc);
		particleArray[i].SetYAcc(yAcc);
		particleArray[i].SetZAcc(zAcc);

		particleArray[i].SetXSpeed(xSpeed);
		particleArray[i].SetYSpeed(ySpeed);
		particleArray[i].SetZSpeed(zSpeed);
	}

	return particleArray;

}