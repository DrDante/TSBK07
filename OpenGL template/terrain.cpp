#include "terrain.h"

//fooo

// terrain variables
int terrainW; //width
int terrainH; //height

// Model
Model *terrain;

// textures
TextureData ttex; // terrain
//GLuint groundTex;

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
			vertexArray[(x + z * tex->width) * 3 + 1] = tex->imageData[(x + z * tex->width) * (tex->bpp / 8)] / 10.0;
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


vec3 giveNormal(int x, int y, int z, GLfloat *vertexArray, GLuint *indexArray, int width, int height)
{
	vec3 vertex = { GLfloat(x), GLfloat(y), GLfloat(z)};
	vec3 normal = { 0, 1, 0 };

	vec3 normal1 = { 0, 1, 0 };
	vec3 normal2 = { 0, 1, 0 };
	vec3 normal3 = { 0, 1, 0 };
	vec3 normal4 = { 0, 1, 0 };
	vec3 normal5 = { 0, 1, 0 };
	vec3 normal6 = { 0, 1, 0 };

	if ((x - 1>0) && (z - 1 >0) && (z + 2 < height) && (x + 2 < width))
	{
		vec3 tempVec1 = { vertexArray[indexArray[(x - 1 + (z - 1) * (width - 1)) * 6 + 0] * 3],
			vertexArray[indexArray[(x - 1 + (z - 1) * (width - 1)) * 6 + 0] * 3 + 1],
			vertexArray[indexArray[(x - 1 + (z - 1) * (width - 1)) * 6 + 0] * 3 + 2] };

		vec3 tempVec2 = { vertexArray[indexArray[(x + (z - 1) * (width - 1)) * 6 + 0] * 3],
			vertexArray[indexArray[(x + (z - 1) * (width - 1)) * 6 + 0] * 3 + 1],
			vertexArray[indexArray[(x + (z - 1) * (width - 1)) * 6 + 0] * 3 + 2] };

		vec3 tempVec3 = { vertexArray[indexArray[(x - 1 + (z)* (width - 1)) * 6 + 0] * 3],
			vertexArray[indexArray[(x - 1 + (z)* (width - 1)) * 6 + 0] * 3 + 1],
			vertexArray[indexArray[(x - 1 + (z)* (width - 1)) * 6 + 0] * 3 + 2] };

		vec3 tempVec4 = { vertexArray[indexArray[(x + 1 + (z)* (width - 1)) * 6 + 0] * 3],
			vertexArray[indexArray[(x + 1 + (z)* (width - 1)) * 6 + 0] * 3 + 1],
			vertexArray[indexArray[(x + 1 + (z)* (width - 1)) * 6 + 0] * 3 + 2] };

		vec3 tempVec5 = { vertexArray[indexArray[(x + 1 + (z + 1) * (width - 1)) * 6 + 0] * 3],
			vertexArray[indexArray[(x + 1 + (z + 1) * (width - 1)) * 6 + 0] * 3 + 1],
			vertexArray[indexArray[(x + 1 + (z + 1) * (width - 1)) * 6 + 0] * 3 + 2] };

		vec3 tempVec6 = { vertexArray[indexArray[(x + (z + 1) * (width - 1)) * 6 + 0] * 3],
			vertexArray[indexArray[(x + (z + 1) * (width - 1)) * 6 + 0] * 3 + 1],
			vertexArray[indexArray[(x + (z + 1) * (width - 1)) * 6 + 0] * 3 + 2] };


		normal1 = CrossProduct(VectorSub(tempVec1, vertex), VectorSub(tempVec2, vertex));
		normal2 = CrossProduct(VectorSub(tempVec2, vertex), VectorSub(tempVec4, vertex));
		normal3 = CrossProduct(VectorSub(tempVec5, vertex), VectorSub(tempVec5, vertex));
		normal4 = CrossProduct(VectorSub(tempVec5, vertex), VectorSub(tempVec6, vertex));
		normal5 = CrossProduct(VectorSub(tempVec6, vertex), VectorSub(tempVec3, vertex));
		normal6 = CrossProduct(VectorSub(tempVec3, vertex), VectorSub(tempVec1, vertex));

		normal = Normalize(VectorAdd(normal1, VectorAdd(normal2, VectorAdd(normal3, VectorAdd(normal4, VectorAdd(normal5, normal6))))));

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
