#include "GenerateGridPositions.h"

int nrXPoints;
int nrZPoints;
GLfloat *sizeArray;
int *typeArray;


// Height = zMax.
// Width = xMax.
int* GenerateGridPositions(GLfloat *vertexArray, int xMax, int zMax, int square_size)
{
	GLfloat tempW = xMax / square_size;
	GLfloat tempH = zMax / square_size;

	// Number of grid points in the different directions.
	nrXPoints = floor(tempW)+1;
	nrZPoints = floor(tempH)+1;

	GLfloat x;
	GLfloat z;

	int *positionsArray = (int *)malloc(sizeof(int)*nrXPoints*nrZPoints*3);
	GLfloat *sizeArrayTemp = (GLfloat *)malloc(sizeof(GLfloat)*nrXPoints*nrZPoints * 3);
	int *typeArrayTemp = (int *)malloc(sizeof(int)*nrXPoints*nrZPoints * 3);

	typeArray = typeArrayTemp;
	sizeArray = sizeArrayTemp;

	for (int i = 0; i < nrZPoints; i++)
	{
		for (int j = 0; j< nrXPoints; j++)
		{
			// Tree positions on a grid with a bit of random displacement added.
			x = i*square_size + rand() / 1000;
			z = j*square_size + rand() / 1000;

			// Control edges.
			if (x <= 1)
			{
				x = 5;
			}
			if (z <= 1)
			{
				z = 5;
			}
			if (xMax <= x)
			{
				x = i*square_size-50;
			}
			if (zMax <= z)
			{
				z = j*square_size-50;
			}

			positionsArray[(i + j * nrXPoints) * 3 + 0] = x;
			positionsArray[(i + j * nrXPoints) * 3 + 1] = findHeight(x, z, vertexArray, xMax, zMax);
			positionsArray[(i + j * nrXPoints) * 3 + 2] = z;

			// Random size for the tree.	
			// sizeArray[(i + j * nrXPoints) * 3 + 0] = 15 + rand() / 1000;

			// Rand type 1 = tree, 2 = bush
			GLfloat typeRand = rand();
			if (typeRand > 5000)
			{
				typeArray[(i + j * nrXPoints) * 3 + 0] = 1;
				sizeArray[(i + j * nrXPoints) * 3 + 0] = 15 + rand() / 1000;
			}
			else
			{
				typeArray[(i + j * nrXPoints) * 3 + 0] = 2;
				sizeArray[(i + j * nrXPoints) * 3 + 0] = rand() / 1000;
			}
		}
	}	
	return positionsArray;
}

// Functions to be used from main.
GLfloat* getSizeArray()
{
	return sizeArray;
}

int* getTypeArray()
{
	return typeArray;
}

int getNrXPoints()
{
	return nrXPoints;
}

int getNrZPoints()
{
	return nrZPoints;
}