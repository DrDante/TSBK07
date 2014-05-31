#ifndef particle_h
#define particle_h

#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "math.h"


class particle{
	vec3 pos{ 0.0, 0.0, 0.0 };
	vec3 speed{0.0, 0.0, 0.0};
	vec3 acceleration{ 0.0, 0.0, 0.0 };
	GLfloat timeAlive = 0;
public:
	particle(vec3 pos, GLfloat size)
	{
		pos = pos;
	}

	void UpdateParticle()
	{
		speed.x = speed.x + acceleration.x;
		speed.y = speed.y + acceleration.y;
		speed.z = speed.z + acceleration.z;

		pos.x = pos.x + speed.x;
		pos.y = pos.y + speed.y;
		pos.z = pos.z + speed.z;
	}
	void SetParticleOrigin(vec3 posIn)
	{
		pos = posIn;
	}
	int GetTimeAlive()
	{
		return timeAlive;
	}
	vec3 GetPosition()
	{
		return pos;
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
	void SetXSpeed(GLfloat x)
	{
		speed.x = x;
	}
	void SetYSpeed(GLfloat y)
	{
		speed.y = y;
	}
	void SetZSpeed(GLfloat z)
	{
		speed.z = z;
	}
	void SetXAcc(GLfloat x)
	{
		acceleration.x = x;
	}
	void SetYAcc(GLfloat y)
	{
		acceleration.y = y;
	}
	void SetZAcc(GLfloat z)
	{
		acceleration.z = z;
	}
};

particle* GenerateParticles(int nrOfParticles, int isExplosion);

#endif