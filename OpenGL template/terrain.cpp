#include "terrain.h"

// terrain variables
//int terrainW; //width
//int terrainH; //height
int nrOfTrees;
int nrOfClouds;

// Model
//Model *terrain;

Model* GenerateTerrain(TextureData *tex)
{
	int vertexCount = tex->width * tex->height;
	int triangleCount = (tex->width - 1) * (tex->height - 1) * 2;
	int x, z;

	GLfloat *vertexArray = (GLfloat *)malloc(sizeof(GLfloat)* 3 * vertexCount);
	GLfloat *normalArray = (GLfloat *)malloc(sizeof(GLfloat)* 3 * vertexCount);
	GLfloat *texCoordArray = (GLfloat *)malloc(sizeof(GLfloat)* 2 * vertexCount);
	GLuint *indexArray = (GLuint *)malloc(sizeof(GLuint)* triangleCount * 3);

	vec3 tempNormal = { 0, 0, 0 };

	printf("bpp %d\n", tex->bpp);
	for (x = 0; x < tex->width; x++)
	{
		for (z = 0; z < tex->height; z++)
		{
			// Vertex array. You need to scale this properly
			vertexArray[(x + z * tex->width) * 3 + 0] = x / 1.0;
			vertexArray[(x + z * tex->width) * 3 + 1] = tex->imageData[(x + z * tex->width) * (tex->bpp / 8)] / 25.0;
			vertexArray[(x + z * tex->width) * 3 + 2] = z / 1.0;

			// Texture coordinates. You may want to scale them.
			texCoordArray[(x + z * tex->width) * 2 + 0] = x; // (float)x / tex->width;
			texCoordArray[(x + z * tex->width) * 2 + 1] = z; // (float)z / tex->height;
		}
	}

	for (x = 0; x < tex->width - 1; x++)
	{
		for (z = 0; z < tex->height - 1; z++)
		{
			// Triangle 1
			indexArray[(x + z * (tex->width - 1)) * 6 + 0] = x + z * tex->width;
			indexArray[(x + z * (tex->width - 1)) * 6 + 1] = x + (z + 1) * tex->width;
			indexArray[(x + z * (tex->width - 1)) * 6 + 2] = x + 1 + z * tex->width;
			// Triangle 2
			indexArray[(x + z * (tex->width - 1)) * 6 + 3] = x + 1 + z * tex->width;
			indexArray[(x + z * (tex->width - 1)) * 6 + 4] = x + (z + 1) * tex->width;
			indexArray[(x + z * (tex->width - 1)) * 6 + 5] = x + 1 + (z + 1) * tex->width;
		}
	}

	for (x = 0; x < tex->width; x++)
	{
		for (z = 0; z < tex->height; z++)
		{
			// Normal vectors. You need to calculate these.
			tempNormal = giveNormal(x, tex->imageData[(x + z * tex->width) * (tex->bpp / 8)] / 10.0, z, vertexArray, indexArray, tex->width, tex->height);
			//printf("normal_x %d\n", x);
			normalArray[(x + z * tex->width) * 3 + 0] = -tempNormal.x;
			normalArray[(x + z * tex->width) * 3 + 1] = -tempNormal.y;
			normalArray[(x + z * tex->width) * 3 + 2] = -tempNormal.z;
		}
	}

	// End of terrain generation

	// Create Model and upload to GPU:

	Model* model = LoadDataToModel(
		vertexArray,
		normalArray,
		texCoordArray,
		NULL,
		indexArray,
		vertexCount,
		triangleCount * 3);

	return model;
}


vec3 giveNormal(int x, int y, int z, GLfloat *vertexArray, GLuint *indexArray, int width, int height)	// Returns the normal of a vertex.
{
	vec3 vertex = { GLfloat(x), GLfloat(y), GLfloat(z) };
	vec3 normal = { 0, 1, 0 };

	vec3 normal1 = { 0, 0, 0 };
	vec3 normal2 = { 0, 0, 0 };
	vec3 normal3 = { 0, 0, 0 };
	vec3 normal4 = { 0, 0, 0 };
	vec3 normal5 = { 0, 0, 0 };
	vec3 normal6 = { 0, 0, 0 };

	if ((x > 1) && (z > 1) && (z < height - 2) && (x < width - 2))
	{
		vec3 tempVec1 = { vertexArray[indexArray[((x - 1) + (z - 1) * (width - 1)) * 6 + 0] * 3],
			vertexArray[indexArray[((x - 1) + (z - 1) * (width - 1)) * 6 + 0] * 3 + 1],
			vertexArray[indexArray[((x - 1) + (z - 1) * (width - 1)) * 6 + 0] * 3 + 2] };

		vec3 tempVec2 = { vertexArray[indexArray[((x)+(z - 1) * (width - 1)) * 6 + 0] * 3],
			vertexArray[indexArray[((x)+(z - 1) * (width - 1)) * 6 + 0] * 3 + 1],
			vertexArray[indexArray[((x)+(z - 1) * (width - 1)) * 6 + 0] * 3 + 2] };

		vec3 tempVec3 = { vertexArray[indexArray[((x - 1) + (z)* (width - 1)) * 6 + 0] * 3],
			vertexArray[indexArray[((x - 1) + (z)* (width - 1)) * 6 + 0] * 3 + 1],
			vertexArray[indexArray[((x - 1) + (z)* (width - 1)) * 6 + 0] * 3 + 2] };

		vec3 tempVec4 = { vertexArray[indexArray[((x + 1) + (z)* (width - 1)) * 6 + 0] * 3],
			vertexArray[indexArray[((x + 1) + (z)* (width - 1)) * 6 + 0] * 3 + 1],
			vertexArray[indexArray[((x + 1) + (z)* (width - 1)) * 6 + 0] * 3 + 2] };

		vec3 tempVec5 = { vertexArray[indexArray[((x + 1) + (z + 1) * (width - 1)) * 6 + 0] * 3],
			vertexArray[indexArray[((x + 1) + (z + 1) * (width - 1)) * 6 + 0] * 3 + 1],
			vertexArray[indexArray[((x + 1) + (z + 1) * (width - 1)) * 6 + 0] * 3 + 2] };

		vec3 tempVec6 = { vertexArray[indexArray[((x)+(z + 1) * (width - 1)) * 6 + 0] * 3],
			vertexArray[indexArray[((x)+(z + 1) * (width - 1)) * 6 + 0] * 3 + 1],
			vertexArray[indexArray[((x)+(z + 1) * (width - 1)) * 6 + 0] * 3 + 2] };


		normal1 = CrossProduct(VectorSub(tempVec1, vertex), VectorSub(tempVec2, vertex));
		normal2 = CrossProduct(VectorSub(tempVec3, vertex), VectorSub(tempVec1, vertex));
		vec3 weighted1 = Normalize(VectorAdd(Normalize(normal1), Normalize(normal2)));
		normal3 = CrossProduct(VectorSub(tempVec2, vertex), VectorSub(tempVec4, vertex));
		vec3 weighted2 = Normalize(normal3);
		normal4 = CrossProduct(VectorSub(tempVec4, vertex), VectorSub(tempVec5, vertex));
		normal5 = CrossProduct(VectorSub(tempVec5, vertex), VectorSub(tempVec6, vertex));
		vec3 weighted3 = Normalize(VectorAdd(Normalize(normal4), Normalize(normal5)));
		normal6 = CrossProduct(VectorSub(tempVec6, vertex), VectorSub(tempVec3, vertex));
		vec3 weighted4 = Normalize(normal6);

		normal = Normalize(VectorAdd(weighted1, VectorAdd(weighted2, VectorAdd(weighted3, weighted4))));

	}
	return normal;

}

int getWidth(TextureData *tex)
{
	int width = tex->width;
	return width;
}
int getHeight(TextureData *tex)
{
	int height = tex->height;
	return height;
}

GLfloat findHeight(GLfloat x, GLfloat z, GLfloat *vertexArray, int width, int height){

	GLfloat yheight = 0;

	int vertX1 = floor(x);
	int vertZ1 = floor(z);

	int vertX2 = floor(x) + 1;
	int vertZ2 = floor(z) + 1;

	int vertX3 = 0;
	int vertZ3 = 0;

	if ((vertX1 > 1) && (vertZ1 > 1) && (vertX2 < height - 2) && (vertZ2 < width - 2))
	{

		GLfloat dist1 = vertX1 - x;
		GLfloat dist2 = vertZ1 - z;

		if (dist1 > dist2)
		{
			vertX3 = vertX1;
			vertZ3 = vertZ1 + 1;

		}
		else
		{
			vertX3 = vertX1 + 1;
			vertZ3 = vertZ1;

		}
		GLfloat vertY1 = vertexArray[(vertX1 + vertZ1 * width) * 3 + 1];

		GLfloat vertY2 = vertexArray[(vertX2 + vertZ2 * width) * 3 + 1];

		GLfloat vertY3 = vertexArray[(vertX3 + vertZ3 * width) * 3 + 1];

		vec3 p1 = { vertexArray[(vertX1 + vertZ1 * width) * 3 + 0], vertY1, vertexArray[(vertX1 + vertZ1 * width) * 3 + 2] };
		vec3 p2 = { vertexArray[(vertX2 + vertZ2 * width) * 3 + 0], vertY2, vertexArray[(vertX2 + vertZ2 * width) * 3 + 2] };
		vec3 p3 = { vertexArray[(vertX3 + vertZ3 * width) * 3 + 0], vertY3, vertexArray[(vertX3 + vertZ3 * width) * 3 + 2] };

		vec3 planeNormal = { 0, 0, 0 };

		// This if/else might not be making any difference whatsoever.
		if (dist1 > dist2)
		{
			planeNormal = Normalize(CrossProduct(VectorSub(p2, p1), VectorSub(p3, p1)));
		}
		else
		{
			planeNormal = Normalize(CrossProduct(VectorSub(p3, p1), VectorSub(p2, p1)));
		}

		GLfloat D;
		D = DotProduct(planeNormal, p1);



		yheight = (D - planeNormal.x*x - planeNormal.z*z) / planeNormal.y;

	}
	return yheight;
}



tree* GetForest(GLfloat *vertexArray, int xMax, int zMax, int square_size)
{
	tree* treeArray = (tree *)malloc(sizeof(tree)*xMax*zMax * 3);
	int* positionsArray = (int *)malloc(sizeof(int)*xMax*zMax * 3);
	int* sizeArray = (int *)malloc(sizeof(int)*xMax*zMax * 3);

	positionsArray = GenerateGridPositions(vertexArray, xMax, zMax, square_size);
	sizeArray = getSizeArray();

	for (int i = 0; i < getNrZPoints(); i++)
	{
		
		for (int j = 0; j < getNrXPoints(); j++)
		{
			treeArray[(i + j * getNrXPoints())].SetXPos(positionsArray[(i + j * getNrXPoints()) * 3 + 0]);
			treeArray[(i + j * getNrXPoints())].SetYPos(positionsArray[(i + j * getNrXPoints()) * 3 + 1]);
			treeArray[(i + j * getNrXPoints())].SetZPos(positionsArray[(i + j * getNrXPoints()) * 3 + 2]);
			treeArray[(i + j * getNrXPoints())].SetSize(sizeArray[(i + j * getNrXPoints()) * 3 + 0]);
			//treeArray[(i + j * getNrXPoints())].GenerateHitBox();
			treeArray[(i + j * getNrXPoints())].SetType(1);
		}
	}
	nrOfTrees = getNrZPoints() * getNrXPoints();
	return treeArray;
}

int GetNrOfTrees()
{
	return nrOfTrees;
}

cloud* GetClouds(GLfloat *vertexArray, int xMax, int zMax, int square_size)
{
	cloud* cloudArray = (cloud *)malloc(sizeof(cloud)*xMax*zMax * 3);
	int* positionsArray = (int *)malloc(sizeof(int)*xMax*zMax * 3);
	int* sizeArray = (int *)malloc(sizeof(int)*xMax*zMax * 3);

	positionsArray = GenerateGridPositions(vertexArray, xMax, zMax, square_size);
	sizeArray = getSizeArray();

	for (int i = 0; i < getNrZPoints(); i++)
	{

		for (int j = 0; j < getNrXPoints(); j++)
		{
			cloudArray[(i + j * getNrXPoints())].SetXPos(positionsArray[(i + j * getNrXPoints()) * 3 + 0]);
			cloudArray[(i + j * getNrXPoints())].SetYPos(5);
			cloudArray[(i + j * getNrXPoints())].SetZPos(positionsArray[(i + j * getNrXPoints()) * 3 + 2]);
			cloudArray[(i + j * getNrXPoints())].SetSize(sizeArray[(i + j * getNrXPoints()) * 3 + 0]);
			//treeArray[(i + j * getNrXPoints())].GenerateHitBox();
			cloudArray[(i + j * getNrXPoints())].SetType(1);
		}
	}
	nrOfClouds = getNrZPoints() * getNrXPoints();
	return cloudArray;
}

int GetNrOfClouds()
{
	return nrOfClouds;
}
