#define _CRT_SECURE_NO_WARNINGS

// Includes.
#include <AL\al.h>
#include <AL\alc.h>
#include <iostream>
#include <gl\glew.h>
#include <gl\freeglut.h>
#include <math.h>
#include "GL_utilities.h"
#include "loadobj.h"
#include "VectorUtils3.h"
#include "LoadTGA.h"
#include "terrain.h"
#include "plane.h"
#include "GenerateGridPositions.h"
#include "tree.h"
#include "cloud.h"
#include "cube.h"
#include "particle.h"


// -----------Globals-----------
#define PI 3.14159265358979323846

// Frustum.
#define near 0.9
#define far 400.0	// Drawing distance.
#define right 0.5
#define left -0.5
#define top 0.5
#define bottom -0.5
GLfloat projMatrix[] = { 2.0f*near / (right - left), 0.0f, (right + left) / (right - left), 0.0f,
0.0f, 2.0f*near / (top - bottom), (top + bottom) / (top - bottom), 0.0f,
0.0f, 0.0f, -(far + near) / (far - near), -2 * far*near / (far - near),
0.0f, 0.0f, -1.0f, 0.0f }; // Projection matrix.

// OpenAL stuff.
ALCdevice *device;
ALCcontext *context;
ALuint source1;
ALuint source2;
ALuint source3;
ALuint buffer1;
ALuint buffer2;
ALuint buffer3;
ALuint frequency;
ALenum format = 0;

ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };									//Position of the source sound.
ALfloat SourceVel[] = { 0.0, 0.0, 0.0 };									//Velocity of the source sound.
ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };									//Position of the listener.
ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };									//Velocity of the listener.
ALfloat ListenerOri[] = { 0.0, 0.0, -1.0, 0.0, 1.0, 0.0 };					//Orientation of the listener.
																			//First direction vector, then vector pointing up.
// Stuff for loading the sound files.
FILE *fp = NULL;
char type[4];
DWORD size, chunkSize;
short formatType, channels;
DWORD sampleRate, avgBytesPerSec;
short bytesPerSample, bitsPerSample;
DWORD dataSize;
bool wasPlayingSound1 = false;
bool wasPlayingSound2 = false;
unsigned char* buf;

// Time variable.
GLfloat t;

// Camera matrices.
mat4 camMatrix;
GLfloat skyMatrix[16];

// Camera variables.
vec3 p = vec3(10.0, 25.0, 20.0);	// Camera placement.
vec3 l = vec3(10.0, 25.0, 0.0);		// Point the camera is looking at.
vec3 v = vec3(0.0, 1.0, 0.0);		// "Up" vector for the camera.
vec3 s = l - p;						// "Forward" vector for the camera.

// Reference to shader programs.
GLuint program;
GLuint particleProgram;
GLuint skyboxProgram;

// Function declarations.
void UploadAndDraw(GLfloat totalMat[], Model *currentModel, bool isSkybox, bool isLambertian);
void CheckMouse(int x, int y);
void SetCameraVector(float fi, float theta);
void CheckKeys();
vec3 ConfinedUpVector(vec3 forward);
mat4 RotatePlaneModel();
vec3 CameraPlacement(vec3 stiffPos, vec3 upVec, vec3 rightVec);
void InitAfterCrash();
void CheckIfOutsideBounderies(vec3 pos);
void TurnPlaneInside(vec3 pos);
void LoadSoundStuff(int fnum);
void CleanUpSoundStuff(int fnum);
bool CheckCollisionWithGround(GLfloat x, GLfloat y, GLfloat z);
void PrintGameInfo();



// Score count.
int scoreCount = 0;

GLint count = 0;

// Glew initialization... thing.
GLenum err;

// -------------------Light source(s)-------------------
const int numberOfLightSources = 4;	// Change here and in the fragment shader when setting the number of light sources.
Point3D lightSourcesColorsArr[] = { { 1.0f, 1.0f, 1.0f },	// White light.
									{ 0.0f, 0.0f, 0.0f },	// No light.
									{ 0.0f, 0.0f, 0.0f },	// No light.
									{ 0.0f, 0.0f, 0.0f } };	// No light.
GLfloat specularExponent[] = { 280.0, 0.0, 0.0, 0.0 };
GLint isDirectional[] = { 1, 0, 0, 0 };
Point3D lightSourcesDirectionsPositions[] = { { -0.58f, 0.58f, -0.58f },	// White light, directional.
											{ 0.0f, 0.0f, 0.0f },			// No light.
											{ 0.0f, 0.0f, 0.0f },			// No light.
											{ 0.0f, 0.0f, 0.0f } };			// No light.
// -----------------------------------------------------

// Models.
Model *skybox;
Model *bunny;
Model *teapot;
Model *terrain;
Model *plane;
Model *planeRot;
Model *trunk;
Model *leaves;
// Rudders.
Model *botLAil;
Model *topLAil;
Model *botRAil;
Model *topRAil;
Model *elevator;
Model *rudder;

// Matrices for the models.
mat4 teapotTrans;
mat4 statTotal;
mat4 botLAilMat, topLAilMat, botRAilMat, topRAilMat, elevatorMat, rudderMat;
mat4 planeTrans;
mat4 propTotal;

// References to textures.
GLuint skyTex;
GLuint bunnyTex, teapotTex;
GLuint trunkTex, leafTex;
GLuint mtnTex1, mtnTex2, mtnTex3;
GLuint boxTex;

// Terrain.
TextureData ttex;

int terrainW;
int terrainH;

tree* treeArray;
cloud* cloudArray; // Not currentyl used.
cube* cubeArray;

// "Particles".
particle* particleCubeArray;
particle* particleExplosionArray;
int nrOfParticles = 150;
int nrOfExplosionParticles = 8000;
bool collisionFirstLoop = TRUE;
bool cubeCollisionFirstLoop = TRUE;

// Triangles.
GLfloat triangleSize = 0.2;
GLfloat triangle[] =
{
	-triangleSize, -triangleSize, 0.0f,
	-triangleSize, triangleSize, 0.0f,
	triangleSize, -triangleSize, 0.0f
};

GLfloat triangleExplosionSize = 0.15;
GLfloat triangleExplosion[] =
{
	-triangleExplosionSize, -triangleExplosionSize, 0.0f,
	-triangleSize, triangleExplosionSize, 0.0f,
	triangleExplosionSize, -triangleExplosionSize, 0.0f
};

GLfloat triangleCubeColor[] =
{
	1.0, 0.0, 0.0f, 1.0,
	0.0, 1.0, 0.0f, 1.0,
	0.0, 0.0, 1.0f, 1.0
};

GLfloat triangleExplosionColor[] =
{
	1.0, 0.0, 0.0f, 1.0,
	1.0, 1.0, 0.0f, 1.0,
	1.0, 0.5, 0.0f, 1.0
};


GLfloat numberSign[] =
{
	0.0f, 0.0f, 0.0f,
	0.0f, 5.0f, 0.0f,
	5.0f, 0.0f, 0.0f
};

GLfloat numberSignTexCoord[] =
{
	0.12f, 0.12f,
	0.12f, 0.0f, 
	0.0f, 0.12f
};

GLfloat numberSign2[] =
{
	5.0f, 5.0f, 0.0f,
	0.0f, 5.0f, 0.0f,
	5.0f, 0.0f, 0.0f
};

GLfloat numberSignTexCoord2[] =
{
	0.0f, 0.0f,
	0.12f, 0.0f,
	0.0f, 0.12f
}; 


// Cube.
unsigned int vertexArrayObjID;
unsigned int vertexArrayObjID2;
unsigned int vertexArrayObjID3;
unsigned int vertexArrayObjID4;

unsigned int vertexBufferObjID6;
unsigned int vertexBufferObjID8;

// ---Bools---
// Plane-crash flags.
bool isExplosion = FALSE;
bool isCubeExplosion = FALSE;
bool playedThisLoop = false;		// Used when looping the plane sound.

// Border control flags.
bool isOutside = FALSE;
bool isTurningInsideRight = FALSE;
bool isTurningInsideLeft = FALSE;
bool isTuningInsideDown = FALSE;

bool wasRollingRight = false;
bool wasRollingLeft = false;
// -----------

// Other global variables.
float yawCamOffset = 0.0;
float pitchCamOffset = 0.0;
float rudderOffset = 0.0;
float elevatorOffset = 0.0;
float ailOffset = 0.0;
GLfloat planeScale = 0.2;			// Plane scale (size).

// Constants.
const float pitchSpeed = 0.02;		// Angular velocity at which the plane pitches.
const float yawSpeed = 0.01;		// Angular velocity at which the plane yaws.
const float rollSpeed = 0.04;		// Angular velocity at which the plane rolls.
const float returnSpeed = 0.02;		// Angular velocity at which the plane returns to upright position from roll.
// ---Camera "sluggishness"---
const float camSpeed = 0.01;		// Do not change.
const float camReturnSpeed = 0.01;	// Do not change.
const float pitchCamLimit = 0.2;	// Do not change.
const float yawCamLimit = 0.2;		// Do not change.
// ---------------------------
const float minSpeed = 0.1;			// Minimum speed of the plane.
const float maxSpeed = 1.0;			// Maximum speed of the plane.
const float startPropSpeed = 0.3;	// Starting speed of the propeller.
const float maxRudder = 0.5;		// Maximum angular offset of the rudder.
const float maxElevator = 0.8;		// Maximum angular offset of the elevator.
const float maxAil = 0.8;			// Maximum angular offset of the aileron.
const int maxFlyingHeight = 250;	// Maximum flying height.

// Plane starting properties.
float propSpeed = startPropSpeed;
Plane player(vec3(40.0, 40.0, 40.0), vec3(1.0, 0.0, 0.0), propSpeed);

// -----------------------------

void init(void)
{
	err = glewInit();

	// Sound stuff.
	// Loading and plaing the plane sound.
	LoadSoundStuff(3);
	alSourcePlay(source3);
	if (alGetError() != AL_NO_ERROR)
		printf("Error playing sound");
	CleanUpSoundStuff(3);

	dumpInfo();

	// GL inits.
	glClearColor(0.43921, 0.57647, 0.858824, 0);
	glEnable(GL_DEPTH_TEST);
	printError("GL inits");

	// Loading and compiling shaders.
	program = loadShaders("main.vert", "main.frag");
	skyboxProgram = loadShaders("skybox.vert", "skybox.frag");


	printError("init shader");

	// ----------------------OBJECT(S)----------------------
	// Load terrain data
	LoadTGATextureData("terrain/arnoldterrang_mountain_edges256.tga", &ttex);
	terrain = GenerateTerrain(&ttex, 2);
	terrainW = getWidth(&ttex);
	terrainH = getHeight(&ttex);
	printError("init terrain");
	
	// Loading models.
	skybox = LoadModelPlus("models/skybox.obj");
	bunny = LoadModelPlus("models/kub.obj");
	teapot = LoadModelPlus("models/teapot.obj");
	plane = LoadModelPlus("models/PlaneWithoutRudders.obj");
	planeRot = LoadModelPlus("models/Blade.obj");
	trunk = LoadModelPlus("models/stamm.obj");
	leaves = LoadModelPlus("models/blad.obj");
	botLAil = LoadModelPlus("models/Roder/L_BotAileron.obj");
	topLAil = LoadModelPlus("models/Roder/R_TopAileron.obj");
	botRAil = LoadModelPlus("models/Roder/R_BotAileron.obj");
	topRAil = LoadModelPlus("models/Roder/L_TopAileron.obj");
	elevator = LoadModelPlus("models/Roder/Elevator.obj");
	rudder = LoadModelPlus("models/Roder/Rudder.obj");

	// Loading textures.
	LoadTGATextureSimple("textures/Skybox512.tga", &skyTex);
	LoadTGATextureSimple("textures/dirt.tga", &bunnyTex);
	LoadTGATextureSimple("textures/rutor.tga", &teapotTex);
	LoadTGATextureSimple("textures/treetext2.tga", &trunkTex);
	LoadTGATextureSimple("textures/leaftext.tga", &leafTex);
	LoadTGATextureSimple("textures/boxtex.tga", &boxTex);
	LoadTGATextureSimple("textures/dirt.tga", &mtnTex1);
	LoadTGATextureSimple("textures/grass.tga", &mtnTex2);
	LoadTGATextureSimple("textures/conc.tga", &mtnTex3);
	// -----------------------------------------------------

	// Initializing "keyboard".
	initKeymapManager();

	// "Initializing" camera.
	s = Normalize(s);
	camMatrix = lookAtv(p, l, v);

	// Initialize forest.
	treeArray = GetForest(terrain->vertexArray, terrainW, terrainH, 50);
	cloudArray = GetClouds(terrain->vertexArray, terrainW, terrainH, 60); // Not currently used.
	cubeArray = GetCubes(terrain->vertexArray, terrainW, terrainH, 50, treeArray, GetNrOfTrees());

	//"Particles"
	particleCubeArray = GenerateParticles(nrOfParticles, 0);
	particleExplosionArray = GenerateParticles(nrOfExplosionParticles,1);

	particleProgram = loadShaders("particle.vert", "particle.frag");
	unsigned int vertexBufferObjID;
	unsigned int vertexBufferObjID2;
	unsigned int vertexBufferObjID3;
	unsigned int vertexBufferObjID4;
	unsigned int vertexBufferObjID5;
	unsigned int vertexBufferObjID7;

	// Taken from lab 1.
	// Allocate and activate Vertex Array Object
	glGenVertexArrays(1, &vertexArrayObjID);
	glBindVertexArray(vertexArrayObjID);
	// Allocate Vertex Buffer Objects.
	glGenBuffers(1, &vertexBufferObjID);
	glGenBuffers(1, &vertexBufferObjID2);
	// VBO for vertex data.
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), triangle, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(particleProgram, "inPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(glGetAttribLocation(particleProgram, "inPosition"));

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID2);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), triangleCubeColor, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(particleProgram, "inColor"), 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(glGetAttribLocation(particleProgram, "inColor"));

	// Allocate and activate Vertex Array Object.
	glGenVertexArrays(1, &vertexArrayObjID2);
	glBindVertexArray(vertexArrayObjID2);
	// Allocate Vertex Buffer Objects.
	glGenBuffers(1, &vertexBufferObjID3);
	glGenBuffers(1, &vertexBufferObjID4);

	// VBO for vertex data.
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID3);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), triangleExplosion, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(particleProgram, "inPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(glGetAttribLocation(particleProgram, "inPosition"));

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID4);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), triangleExplosionColor, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(particleProgram, "inColor"), 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(glGetAttribLocation(particleProgram, "inColor"));

	// Allocate and activate Vertex Array Object.
	glGenVertexArrays(1, &vertexArrayObjID3);
	glBindVertexArray(vertexArrayObjID3);
	// Allocate Vertex Buffer Objects.
	glGenBuffers(1, &vertexBufferObjID5);
	glGenBuffers(1, &vertexBufferObjID6);

	// VBO for vertex data.
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID5);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), numberSign, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(program, "inPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(glGetAttribLocation(program, "inPosition"));
	
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID6);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), numberSignTexCoord, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(program, "inTexCoord"), 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(glGetAttribLocation(program, "inTexCoord"));

	// Allocate and activate Vertex Array Object.
	glGenVertexArrays(1, &vertexArrayObjID4);
	glBindVertexArray(vertexArrayObjID4);
	// Allocate Vertex Buffer Objects.
	glGenBuffers(1, &vertexBufferObjID7);
	glGenBuffers(1, &vertexBufferObjID8);

	// VBO for vertex data.
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID7);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), numberSign2, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(program, "inPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(glGetAttribLocation(program, "inPosition"));
	
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID8);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), numberSignTexCoord2, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(program, "inTexCoord"), 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(glGetAttribLocation(program, "inTexCoord"));

	glUniform1i(glGetUniformLocation(program, "multitex"), 0);

	// -------Other initializations-------
	statTotal = T(0.0, 0.0, 0.0);	// Placement of the terrain pieces.

	// Lighting stuff.
	glUseProgram(program);
	glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), numberOfLightSources, &lightSourcesDirectionsPositions[0].x);
	glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), numberOfLightSources, &lightSourcesColorsArr[0].x);
	glUniform1fv(glGetUniformLocation(program, "specularExponent"), numberOfLightSources, specularExponent);
	glUniform1iv(glGetUniformLocation(program, "isDirectional"), numberOfLightSources, isDirectional);
	glActiveTexture(GL_TEXTURE0);
	// -----------------------------------

	PrintGameInfo();
}

void display(void)
{
	printError("pre display");

	// Update t for the current elapsed time.
	t = (GLfloat)glutGet(GLUT_ELAPSED_TIME);
	for (int i = 0; i < 50; i++)
	{
		if (int(t) % 7500 == i && !playedThisLoop)
		{
			playedThisLoop = true;
			LoadSoundStuff(3);
			alSourcePlay(source3);
			if (alGetError() != AL_NO_ERROR)
				printf("Error playing sound");
			CleanUpSoundStuff(3);
		}
		if (int(t) % 3250 == i && playedThisLoop)
		{
			playedThisLoop = false;
		}
	}
	CheckKeys();

	// Clear the screen.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Skybox.
	glUseProgram(skyboxProgram);
	glBindTexture(GL_TEXTURE_2D, skyTex);
	UploadAndDraw(skyMatrix, skybox, 1, 0);
	glUseProgram(program);

	// Uploading the camera position (in world coordinates) to the shaders.
	GLfloat camPos[3] = { p.x, p.y, p.z };
	glUniform3fv(glGetUniformLocation(program, "camPos"), 1, camPos);

	// ----------------------Object(s)----------------------
	// -----Terrain-----
	glUniform1i(glGetUniformLocation(program, "texUnit"), 0);
	glUniform1i(glGetUniformLocation(program, "texUnit2"), 1);
	glUniform1i(glGetUniformLocation(program, "texUnit3"), 2);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mtnTex1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mtnTex2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, mtnTex3);
	glUniform1i(glGetUniformLocation(program, "multitex"), 1);
	UploadAndDraw(statTotal.m, terrain, 0, 1);
	glUniform1i(glGetUniformLocation(program, "multitex"), 0);
	glActiveTexture(GL_TEXTURE0);
	// -----------------
	
	// ------Plane------
	// Plane moves, unless it's exploded.
	if (!isExplosion)
	{
	player.MovePlane();
	}

	// Check if plane has collided with ground.
	if (CheckCollisionWithGround(player.GetPosition().x, player.GetPosition().y, player.GetPosition().z))
	{
		isExplosion = TRUE;
		player.SetCollision(TRUE);

		if (collisionFirstLoop)
		{
			for (int i = 0; i < nrOfExplosionParticles; i++)
			{
				particleExplosionArray[i].SetParticleOrigin(player.GetPosition());
			}
		}
		collisionFirstLoop = FALSE;
	}

	// Rotating model.
	mat4 PlaneMatrix = RotatePlaneModel();

	// Moving model.
	PlaneMatrix = Mult(Mult(T(player.GetPosition().x, player.GetPosition().y, player.GetPosition().z), PlaneMatrix), S(planeScale, planeScale, planeScale));

	// Aileron.
	mat4 lAilRot = Rx(ailOffset);
	lAilRot = Mult(Rz(-0.072), lAilRot);
	botLAilMat = Mult(T(-2.16824, -0.371189, -0.2), lAilRot);
	botLAilMat = Mult(PlaneMatrix, botLAilMat);
	lAilRot = ArbRotate(Normalize(vec3(5.0, -0.2, 0.8)), ailOffset);
	topLAilMat = Mult(T(-2.65974, 1.01, 0.0), lAilRot);
	topLAilMat = Mult(PlaneMatrix, topLAilMat);
	mat4 rAilRot = Rx(-ailOffset);
	rAilRot = Mult(Rz(0.072), rAilRot);
	botRAilMat = Mult(T(2.16824, -0.371189, -0.2), rAilRot);
	botRAilMat = Mult(PlaneMatrix, botRAilMat);
	rAilRot = ArbRotate(Normalize(vec3(5.0, 0.2, -0.8)), -ailOffset);
	topRAilMat = Mult(T(2.65974, 1.01, 0.0), rAilRot);
	topRAilMat = Mult(PlaneMatrix, topRAilMat);
	// Elevator.
	mat4 elevatorRot = Rx(elevatorOffset);
	elevatorMat = Mult(T(0.0, 0.1818, -2.83), elevatorRot);
	elevatorMat = Mult(PlaneMatrix, elevatorMat);
	// Rudder.
	mat4 rudderRot = Ry(rudderOffset);
	rudderMat = Mult(T(0.0, 0.3518, -2.83), rudderRot);
	rudderMat = Mult(PlaneMatrix, rudderMat);

	// Turns player back if outside the levels boundaries.
	CheckIfOutsideBounderies(player.GetPosition());
	if (isOutside){
		TurnPlaneInside(player.GetPosition());
	}

	// Blades.
	mat4 propRot = Rz(propSpeed * 0.1 * t);
	propTotal = Mult(PlaneMatrix, propRot);

	// Animates the movable parts of the plane unless it's exploded.
	if (!isExplosion){
		glBindTexture(GL_TEXTURE_2D, skyTex);
		UploadAndDraw(PlaneMatrix.m, plane, 0, 0);
		UploadAndDraw(propTotal.m, planeRot, 0, 0);
		UploadAndDraw(botLAilMat.m, botLAil, 0, 0);
		UploadAndDraw(topLAilMat.m, topLAil, 0, 0);
		UploadAndDraw(botRAilMat.m, botRAil, 0, 0);
		UploadAndDraw(topRAilMat.m, topRAil, 0, 0);
		UploadAndDraw(elevatorMat.m, elevator, 0, 0);
		UploadAndDraw(rudderMat.m, rudder, 0, 0);
	}

	// Camera stuff.
	s = player.GetDirection();	// Forward vector.
	l = player.GetPosition();	// What the camera is looking at.
	p = l - s * 5.0;			// Stiff camera placement, 5.0 behind the plane.
	vec3 sluggishCamPos = p;
	// Calculates a "sluggish" camera, one that (gives the effect that it) doesn't follow the plane immediately when it turns.
	if (!isExplosion)
	{
		sluggishCamPos = l + CameraPlacement(p, player.GetUpVector(), Normalize(CrossProduct(player.GetDirection(), player.GetUpVector())));
	}
	else
	{
		pitchCamOffset = 0.0;
	}
	v = ConfinedUpVector(player.GetDirection());
	camMatrix = lookAtv(sluggishCamPos, l, v);
	// -----------------

	// Trees.
	mat4 treeTrans;
	mat4 treeRot;
	mat4 treeScale;
	mat4 treeTotal;

	for (int i = 0; i < GetNrOfTrees(); i++)
	{
		treeTrans = T(treeArray[i].GetPosition().x, treeArray[i].GetPosition().y, treeArray[i].GetPosition().z);
		treeRot = Rx(-PI*0.5);
		treeScale = S(treeArray[i].GetSize(), treeArray[i].GetSize(), treeArray[i].GetSize());

		treeTotal = Mult(treeRot, treeScale);
		treeTotal = Mult(treeTrans, treeTotal);
		glBindTexture(GL_TEXTURE_2D, trunkTex);
		UploadAndDraw(treeTotal.m, trunk, 0, 0);
		glBindTexture(GL_TEXTURE_2D, leafTex);
		UploadAndDraw(treeTotal.m, leaves, 0, 0);

		if (treeArray[i].CheckHitBox(player.GetPosition())) // Check for player collision with tree.
		{
			isExplosion = TRUE;
			player.SetCollision(TRUE);

			if (collisionFirstLoop)
			{
				for (int i = 0; i < nrOfExplosionParticles; i++)
				{
					particleExplosionArray[i].SetParticleOrigin(player.GetPosition());

				}
			}
			collisionFirstLoop = FALSE;
		}
	}
	// Cubes.
	mat4 cubeTrans;
	glUseProgram(program);
	for (int i = 0; i < GetNrOfCubes(); i++)
	{
		bool hit = cubeArray[i].AlreadyHit();
		if (hit)
		{		
			cubeTrans = T(cubeArray[i].GetPosition().x, cubeArray[i].GetPosition().y, cubeArray[i].GetPosition().z);
			glBindTexture(GL_TEXTURE_2D, boxTex);
			UploadAndDraw(cubeTrans.m, bunny, 0, 0);

			if (cubeArray[i].CheckHitBox(player.GetPosition())) // Check for player collision with cube.
			{
				isCubeExplosion = TRUE;
				scoreCount += 1;
				propSpeed += 0.1;
				player.SetVelocity(propSpeed);
				int textureLine = 0;
				if (scoreCount > 8)
				{
					textureLine = 1;
				}
				if (scoreCount >= 1)
				{
					GLfloat numberSignTexCoord[] =
					{
						0.125*scoreCount, 0.125*(textureLine+1),
						0.125*scoreCount, 0.125*textureLine,
						0.125*(scoreCount - 1), 0.125*(textureLine + 1)
					};
					glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID6);
					glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), numberSignTexCoord, GL_STATIC_DRAW);
					glVertexAttribPointer(glGetAttribLocation(program, "inTexCoord"), 2, GL_FLOAT, GL_FALSE, 0, 0);
					glEnableVertexAttribArray(glGetAttribLocation(program, "inTexCoord"));
					GLfloat numberSignTexCoord2[] =
					{
						0.125*(scoreCount - 1), 0.125*(textureLine ),
						0.125*scoreCount, 0.125*(textureLine ),
						0.125*(scoreCount - 1), 0.125*(textureLine + 1)
					};

					glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID8);
					glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), numberSignTexCoord2, GL_STATIC_DRAW);
					glVertexAttribPointer(glGetAttribLocation(program, "inTexCoord"), 2, GL_FLOAT, GL_FALSE, 0, 0);
					glEnableVertexAttribArray(glGetAttribLocation(program, "inTexCoord"));
				}
				if (cubeCollisionFirstLoop)
				{
					for (int i = 0; i < nrOfParticles; i++)
					{
						particleCubeArray[i].SetParticleOrigin(player.GetPosition());
					}
				}
				cubeCollisionFirstLoop = FALSE;
			}
		}
	}

	// Cube explosion.
	glUseProgram(particleProgram);
	mat4 particleTrans;
	if (isCubeExplosion)
	{
		// Sound stuff.
		// Playing the sound for picking up the cubes.
		if (!wasPlayingSound2)
		{
			LoadSoundStuff(2);
			wasPlayingSound2 = true;
			alSourcePlay(source2);
			if (alGetError() != AL_NO_ERROR)
				printf("Error playing sound");
		}

		for (int i = 0; i < nrOfParticles; i++)
		{
			particleTrans = T(particleCubeArray[i].GetPosition().x, particleCubeArray[i].GetPosition().y - 2, particleCubeArray[i].GetPosition().z);
			particleCubeArray[i].UpdateParticle();
			// Drawing vertices.
			glUniformMatrix4fv(glGetUniformLocation(particleProgram, "MTWMatrix"), 1, GL_TRUE, particleTrans.m);
			glUniformMatrix4fv(glGetUniformLocation(particleProgram, "WTVMatrix"), 1, GL_TRUE, camMatrix.m);
			glUniformMatrix4fv(glGetUniformLocation(particleProgram, "VTPMatrix"), 1, GL_TRUE, projMatrix);
			glBindVertexArray(vertexArrayObjID);	// Selecting VAO.
			glDrawArrays(GL_TRIANGLES, 0, 3);		// Drawing object.
		}

		count += 1;
		if (count > 80){
			CleanUpSoundStuff(2);
			isCubeExplosion = FALSE;
			cubeCollisionFirstLoop = TRUE;
			particleCubeArray = GenerateParticles(nrOfParticles, 0);
			count = 0;
		}
	}

	// Plane explosion.
	if (isExplosion)
	{
		// Sound stuff.
		// Playing the plane explosion sound.
		if (!wasPlayingSound1)
		{
			LoadSoundStuff(1);
			wasPlayingSound1 = true;
			alSourcePlay(source1);
			if (alGetError() != AL_NO_ERROR)
				printf("Error playing sound");
		}

		for (int i = 0; i < nrOfExplosionParticles; i++)
		{	
			particleTrans = T(particleExplosionArray[i].GetPosition().x, particleExplosionArray[i].GetPosition().y - 2, particleExplosionArray[i].GetPosition().z);
			particleExplosionArray[i].UpdateParticle();
		// Drawing vertices.
		glUniformMatrix4fv(glGetUniformLocation(particleProgram, "MTWMatrix"), 1, GL_TRUE, particleTrans.m);
		glUniformMatrix4fv(glGetUniformLocation(particleProgram, "WTVMatrix"), 1, GL_TRUE, camMatrix.m);
		glUniformMatrix4fv(glGetUniformLocation(particleProgram, "VTPMatrix"), 1, GL_TRUE, projMatrix);
		glBindVertexArray(vertexArrayObjID2);	// Selecting VAO.
		glDrawArrays(GL_TRIANGLES, 0, 3);		// Drawing object.
		}
		count += 1;
		if (count > 85)
		{
			InitAfterCrash();
			count = 0;
		}
	}

	// -----Score counter-----
	glUseProgram(skyboxProgram);

	// Score counter matrices.
	mat4 signTrans;
	mat4 signScale;
	mat4 signRot;
	mat4 signTot;
	
	vec3 camForward = Normalize(l - sluggishCamPos); // Forward vector for the camera, accounting for sluggishness.
	vec3 tempV=MultVec3(ArbRotate(Normalize(CrossProduct(player.GetDirection(), player.GetUpVector())), pitchCamOffset),v);
	tempV = MultVec3(ArbRotate(Normalize(player.GetUpVector()), yawCamOffset), tempV); // Up vector for the camera, compensated for pitch and yaw offset.
	vec3 camLeft = Normalize(CrossProduct(tempV, camForward)); // Left vector for the camera.

	signTrans = T(camForward.x, camForward.y, camForward.z); // Placing score board one length unit in front of the camera.
	signTrans = Mult(signTrans, T(-0.5 * Normalize(tempV).x, -0.5 * Normalize(tempV).y, -0.5 * Normalize(tempV).z)); // Placing the score board a bit down.
	signTrans = Mult(signTrans, T(0.4*camLeft.x, 0.4*camLeft.y, 0.4*camLeft.z)); // Placing the board to the side. (Comment this line to place it in the bottom middle.)
	signScale = S(0.015, 0.015, 0.015);
	signRot = Ry(PI);
	signTot = InvertMat4(camMatrix);

	signTot = Mult(signTrans,signTot);
	signTot = Mult(signTot, signRot);
	signTot = Mult(signTot,signScale);

	glBindTexture(GL_TEXTURE_2D, teapotTex);
	glUniformMatrix4fv(glGetUniformLocation(program, "MTWMatrix"), 1, GL_TRUE, signTot.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "WTVMatrix"), 1, GL_TRUE, camMatrix.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "VTPMatrix"), 1, GL_TRUE, projMatrix);

	if (scoreCount > 0)
	{
		glBindVertexArray(vertexArrayObjID3);	// Selecting VAO.
		glDrawArrays(GL_TRIANGLES, 0, 3);	// Drawing object

		glBindVertexArray(vertexArrayObjID4);	// Selecting VAO.
		glDrawArrays(GL_TRIANGLES, 0, 3);	// Drawing object.
	}
	glUseProgram(program);
	// -----------------------

	// Extra objects, a.k.a. Magical Multi-textured Teapot of Mystery, a.k.a testing height function.
	float tempx = 100 + 50 * cosf(0.0005 * t);
	float tempz = 100 + 50 * sinf(0.0005 * t);
	teapotTrans = T(tempx, findHeight(tempx, tempz, terrain->vertexArray, terrainW, terrainH), tempz);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, teapotTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bunnyTex);
	glUniform1i(glGetUniformLocation(program, "multitex"), 1);
	UploadAndDraw(teapotTrans.m, teapot, 0, 0);
	glUniform1i(glGetUniformLocation(program, "multitex"), 0);
	glActiveTexture(GL_TEXTURE0);
	// -----------------------------------------------------

	printError("display");

	glutSwapBuffers();
}

void OnTimer(int value) // Updates the time.
{
	glutPostRedisplay();
	glutTimerFunc(20, &OnTimer, value);
}

void UploadAndDraw(GLfloat totalMat[], Model *currentModel, bool isSkybox, bool isLambertian) // Uploads and draws the specified object.
{
	glUniform1i(glGetUniformLocation(program, "texUnit"), 0);
	glUniformMatrix4fv(glGetUniformLocation(program, "VTPMatrix"), 1, GL_TRUE, projMatrix);
	// Special rules if the object is the skybox.
	if (isSkybox)
	{
		for (int i = 0; i < 16; i++)
		{
			skyMatrix[i] = camMatrix.m[i];
		}

		// Zeroing out the translation part.
		skyMatrix[3] = 0.0;
		skyMatrix[7] = 0.0;
		skyMatrix[11] = 0.0;
		skyMatrix[15] = 1.0;

		// Preparations for drawing the skybox.
		glDisable(GL_DEPTH_TEST);
		
		glUniform1i(glGetUniformLocation(skyboxProgram, "texUnit"), 0);
		glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "VTPMatrix"), 1, GL_TRUE, projMatrix);
		glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "MTWMatrix"), 1, GL_TRUE, IdentityMatrix().m);
		glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "WTVMatrix"), 1, GL_TRUE, skyMatrix);

		DrawModel(currentModel, skyboxProgram, "inPosition", "inNormal", "inTexCoord");
	}
	else
	{
		glUniformMatrix4fv(glGetUniformLocation(program, "MTWMatrix"), 1, GL_TRUE, totalMat);
		glUniformMatrix4fv(glGetUniformLocation(program, "WTVMatrix"), 1, GL_TRUE, camMatrix.m);
	}
	if (isLambertian)
	{
		glUniform1i(glGetUniformLocation(program, "lambert"), 1); // Disables specular lighting.
	}
	// Drawing the model.
	DrawModel(currentModel, program, "inPosition", "inNormal", "inTexCoord");
	if (isLambertian)
	{
		glUniform1i(glGetUniformLocation(program, "lambert"), 0); // Re-enables specular lighting.
	}
	if (isSkybox)
	{
		// Restoring some changes made when the skybox was drawn.
		glEnable(GL_DEPTH_TEST);
		glUniform1i(glGetUniformLocation(program, "skybox"), 0);
	}
}

void CheckMouse(int x, int y) // Aligns camera direction after mouse cursor location. Not currently used.
{
	float xSensMultiplier = 2.0;
	float fi = 2 * PI * (float)x / glutGet(GLUT_WINDOW_WIDTH);
	float theta = PI * (float)y / glutGet(GLUT_WINDOW_HEIGHT);
	//SetCameraVector(xSensMultiplier * (PI - fi), -theta);
}

void SetCameraVector(float fi, float theta) // Sets the camera matrix, depending on the position of the mouse. Not currently used.
{
	// Sets s, the direction you're looking at.
	s.z = sinf(theta) * cosf(fi);
	s.x = sinf(theta) * sinf(fi);
	s.y = cosf(theta);
	// Translates this into l, the point "just in front of your face" when looking along s.
	l = p + s;
	camMatrix = lookAtv(p, l, v);
}

void CheckKeys() // Checks if keys are being pressed.
{
	vec3 tempRight = Normalize(CrossProduct(player.GetDirection(), player.GetUpVector()));
	// 'p' resets the game.
	if (keyIsDown('p'))
	{
		InitAfterCrash();
	}
	if (!isOutside) // If you're out of bounds, and the plane is turning back in, resistance is futile.
	{
		// 'w' pitches the plane downwards.
		if (keyIsDown('w'))
		{
			vec3 tempForward = Normalize(player.GetDirection() - pitchSpeed * player.GetUpVector());
			vec3 tempUp = Normalize(player.GetUpVector() + pitchSpeed * player.GetDirection());
			player.SetDirection(tempForward, tempUp);
			if (pitchCamOffset < pitchCamLimit && !isExplosion)
			{
				pitchCamOffset += camSpeed;
			}
			if (elevatorOffset < maxElevator)
			{
				elevatorOffset += 0.1;
			}
		}
		// 's' pitches the plane upwards.
		if (keyIsDown('s'))
		{
			vec3 tempForward = Normalize(player.GetDirection() + pitchSpeed * player.GetUpVector());
			vec3 tempUp = Normalize(player.GetUpVector() - pitchSpeed * player.GetDirection());
			player.SetDirection(tempForward, tempUp);
			if (pitchCamOffset > -pitchCamLimit && !isExplosion)
			{
				pitchCamOffset -= camSpeed;
			}
			if (elevatorOffset > -maxElevator)
			{
				elevatorOffset -= 0.1;
			}
		}
		// 'a' yaws the plane to the left.
		if (keyIsDown('a'))
		{
			player.SetDirection(Normalize(player.GetDirection() - yawSpeed * tempRight), player.GetUpVector());
			if (yawCamOffset > -yawCamLimit)
			{
				yawCamOffset -= camSpeed;
			}
			if (rudderOffset > -maxRudder)
			{
				rudderOffset -= 0.1;
			}
		}
		// 'd' yaws the plane to the right.
		if (keyIsDown('d'))
		{
			player.SetDirection(Normalize(player.GetDirection() + yawSpeed * tempRight), player.GetUpVector());
			if (yawCamOffset < yawCamLimit)
			{
				yawCamOffset += camSpeed;
			}
			if (rudderOffset < maxRudder)
			{
				rudderOffset += 0.1;
			}
		}
		// 'q' rolls the plane counterclockwise.
		if (keyIsDown('q'))
		{
			player.SetDirection(player.GetDirection(), Normalize(player.GetUpVector() - rollSpeed * tempRight));
			wasRollingRight = false;
			if (ailOffset < maxAil)
			{
				ailOffset += 0.1;
			}
		}
		// 'e' rolls the plane clockwise.
		if (keyIsDown('e'))
		{
			player.SetDirection(player.GetDirection(), Normalize(player.GetUpVector() + rollSpeed * tempRight));
			wasRollingLeft = false;
			if (ailOffset > -maxAil)
			{
				ailOffset -= 0.1;
			}
		}
		// 'r' increases the speed.
		if (keyIsDown('r'))
		{
			if (propSpeed < maxSpeed)
			{
				propSpeed += 0.01;
				if (propSpeed >= minSpeed)
				{
					player.SetVelocity(propSpeed);
				}
			}
			else
			{
				propSpeed = maxSpeed;
			}
		}
		// 'f' decreases the speed.
		if (keyIsDown('f'))
		{
			if (propSpeed > 0.0)
			{
				propSpeed -= 0.01;
				if (propSpeed >= minSpeed)
				{
					player.SetVelocity(propSpeed);
				}
			}
			else
			{
				propSpeed = 0.0;
			}
		}
		// If not currently actively rolling, the roll is slowly reset.
		vec3 yUp = vec3(0.0, 1.0, 0.0);
		if (!keyIsDown('q') && !keyIsDown('e'))
		{
			if (tempRight.y < -0.001)
			{
				if (wasRollingLeft)
				{
					if (DotProduct(player.GetUpVector(), yUp) > 0)
					{
						player.SetDirection(player.GetDirection(), ConfinedUpVector(player.GetDirection()));
						wasRollingLeft = false;
					}
				}
				else
				{
					player.SetDirection(player.GetDirection(), Normalize(player.GetUpVector() - returnSpeed * tempRight));
					wasRollingRight = true;
				}
			}
			else if (tempRight.y > 0.001)
			{
				if (wasRollingRight)
				{
					vec3 yUp = vec3(0.0, 1.0, 0.0);
					if (DotProduct(player.GetUpVector(), yUp) > 0)
					{
						player.SetDirection(player.GetDirection(), ConfinedUpVector(player.GetDirection()));
						wasRollingRight = false;
					}
				}
				else
				{
					player.SetDirection(player.GetDirection(), Normalize(player.GetUpVector() + returnSpeed * tempRight));
					wasRollingLeft = true;
				}
			}
			else if (DotProduct(player.GetUpVector(), yUp) > 0)
			{
				player.SetDirection(player.GetDirection(), ConfinedUpVector(player.GetDirection()));
				wasRollingRight = false;
				wasRollingLeft = false;
			}
			if (ailOffset < -0.1)
			{
				ailOffset += 0.05;
			}
			else if (ailOffset > 0.1)
			{
				ailOffset -= 0.05;
			}
			else
			{
				ailOffset = 0.0;
			}
		}
		// Slowly reduces the pitch camera offset.
		if (!keyIsDown('w') && !keyIsDown('s'))
		{
			if (pitchCamOffset > 0)
			{
				pitchCamOffset -= camReturnSpeed;
			}
			else if (pitchCamOffset < 0)
			{
				pitchCamOffset += camReturnSpeed;
			}
			if (elevatorOffset > 0.1)
			{
				elevatorOffset -= 0.05;
			}
			else if (elevatorOffset < -0.1)
			{
				elevatorOffset += 0.05;
			}
			else
			{
				elevatorOffset = 0.0;
			}
		}
		// Slowly reduces the yaw camera offset.
		if (!keyIsDown('a') && !keyIsDown('d'))
		{
			if (yawCamOffset > 0)
			{
				yawCamOffset -= camReturnSpeed;
			}
			else if (yawCamOffset < 0)
			{
				yawCamOffset += camReturnSpeed;
			}
			if (rudderOffset > 0.1)
			{
				rudderOffset -= 0.05;
			}
			else if (rudderOffset < -0.1)
			{
				rudderOffset += 0.05;
			}
			else
			{
				rudderOffset = 0.0;
			}
		}
	}
}

bool CheckCollisionWithGround(GLfloat x, GLfloat y, GLfloat z) // Checks for collision with ground given object coordinates (x,y,z).
{
	bool isCollision = FALSE;
	GLfloat groundHeight = findHeight(x, z, terrain->vertexArray, terrainW, terrainH);
	if (isnan(groundHeight) == 1) // Outside terrain?
	{
		groundHeight = 0;
	}
	if (groundHeight > y)
	{
		isCollision = TRUE;
	}
	return isCollision;
}

vec3 ConfinedUpVector(vec3 forward) // Returns an "up" vector confined to the plane spanned by the forward vector and the y axis.
{
	vec3 yUp = { 0.0, 1.0, 0.0 };
	return Normalize(yUp - DotProduct(yUp, forward) * forward);
}

mat4 RotatePlaneModel() // Returns a rotation matrix for the plane model.
{
	mat4 PlaneMatrix = IdentityMatrix();
	vec3 tempRight = Normalize(CrossProduct(player.GetDirection(), player.GetUpVector()));
	PlaneMatrix.m[0] = tempRight.x;
	PlaneMatrix.m[4] = tempRight.y;
	PlaneMatrix.m[8] = tempRight.z;
	PlaneMatrix.m[1] = player.GetUpVector().x;
	PlaneMatrix.m[5] = player.GetUpVector().y;
	PlaneMatrix.m[9] = player.GetUpVector().z;
	PlaneMatrix.m[2] = player.GetDirection().x;
	PlaneMatrix.m[6] = player.GetDirection().y;
	PlaneMatrix.m[10] = player.GetDirection().z;
	return PlaneMatrix;
}

vec3 CameraPlacement(vec3 stiffPos, vec3 upVec, vec3 rightVec) // Used to produce a "sluggish" camera.
{
	vec3 planeToCam = stiffPos - player.GetPosition();
	mat4 offsetPitch = ArbRotate(rightVec, pitchCamOffset);
	mat4 offsetYaw = ArbRotate(upVec, yawCamOffset);
	mat4 camResult = Mult(offsetYaw, offsetPitch);
	return vec3(camResult*planeToCam);
}

void InitAfterCrash() // Resets the game (normally when you have crashed).
{
	// Sound stuff
	CleanUpSoundStuff(1);
	
	player.SetCollision(FALSE);
	player.SetDirection(vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));
	player.SetPosition(vec3(40.0, 40.0, 40.0));
	propSpeed = startPropSpeed;
	player.SetVelocity(propSpeed);
	isExplosion = FALSE;
	collisionFirstLoop = TRUE;
	particleExplosionArray = GenerateParticles(nrOfExplosionParticles, 1);
	scoreCount = 0;
	cubeArray = GetCubes(terrain->vertexArray, terrainW, terrainH, 50, treeArray, GetNrOfTrees());
}

void CheckIfOutsideBounderies(vec3 pos) // Checks if the plane is out of bounds.
{
	if (pos.x < 4 || pos.z < 4 || pos.x >terrainH-4 || pos.z>terrainW-4 || pos.y >maxFlyingHeight)
	{
		isOutside = TRUE;
	}
	else
	{
		isOutside = FALSE;
		isTurningInsideRight = FALSE;
		isTurningInsideLeft = FALSE;
		isTuningInsideDown = FALSE;
	}
}

void TurnPlaneInside(vec3 pos) //Turns plane back inside if outside the boundaries of the level.
{
	GLfloat angle = 0;

	vec3 tempRight = Normalize(CrossProduct(player.GetDirection(), player.GetUpVector()));

	if (pos.x < 4 && !isTurningInsideRight && !isTurningInsideLeft)
	{
		angle = PI - acos(DotProduct(vec3{ 0, 0, 1 }, Normalize(player.GetDirection())));
	}
	if (pos.z < 4 && !isTurningInsideRight && !isTurningInsideLeft)
	{
		angle = acos(DotProduct(vec3{ 1, 0, 0 }, Normalize(player.GetDirection())));
	}
	if (pos.x>terrainH - 4 && !isTurningInsideRight && !isTurningInsideLeft)
	{
		angle = acos(DotProduct(vec3{ 0, 0, 1 }, Normalize(player.GetDirection())));
	}
	if (pos.z>terrainW - 4 && !isTurningInsideRight && !isTurningInsideLeft)
	{
		angle = PI - acos(DotProduct(vec3{ 0, 0, 1 }, Normalize(player.GetDirection())));
	}
	// Turning right
	if ((angle < (PI / 2) || isTurningInsideRight) && !isTuningInsideDown)
	{
		player.SetDirection(Normalize(player.GetDirection() + (yawSpeed + 0.08) * tempRight), player.GetUpVector());
		if (yawCamOffset < yawCamLimit)
		{
			yawCamOffset += camSpeed;
		}
		isTurningInsideRight = TRUE;
	}
	// Turning left
	if((angle >= (PI / 2)  || isTurningInsideLeft) && !isTuningInsideDown)
	{
		player.SetDirection(Normalize(player.GetDirection() - (yawSpeed + 0.1) * tempRight), player.GetUpVector());
		if (yawCamOffset > -yawCamLimit)
		{
			yawCamOffset -= camSpeed;
		}
		isTurningInsideLeft = TRUE;
	}
	// Turning down
	if (pos.y > maxFlyingHeight && !isTuningInsideDown)
	{
		vec3 dir = player.GetDirection();
		dir.y = player.GetDirection() .y- 0.01;
		player.SetDirection(dir, CrossProduct(tempRight,dir));
		isTuningInsideDown = TRUE;
	}
	else
	{
		isTuningInsideDown = FALSE;
	}
}

void LoadSoundStuff(int fnum) // Loads the sound data and prepares it for playing.
{
	device = alcOpenDevice(NULL);
	if (!device)
		printf("no sound device");
	context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);
	if (!context)
		printf("no sound context");
	switch (fnum)
	{
	case 1:
		alGenSources((ALuint)1, &source1);
		alGenBuffers((ALuint)1, &buffer1);
		break;
	case 2:
		alGenSources((ALuint)1, &source2);
		alGenBuffers((ALuint)1, &buffer2);
		break;
	case 3:
		alGenSources((ALuint)1, &source3);
		alGenBuffers((ALuint)1, &buffer3);
		break;
	default:
		break;
	}
	if (alGetError() != AL_NO_ERROR)
		printf("Error GenSource");

	switch (fnum)
	{
	case 1:
		fp = fopen("audio/Explosion 2-SoundBible.com-1641389556.wav", "rb");
		break;
	case 2:
		fp = fopen("audio/Metroid_Door-Brandino480-995195341.wav", "rb");
		break;
	case 3:
		fp = fopen("audio/Propeller-SoundBible.com-615398190.wav", "rb");
		break;
	default:
		break;
	}
	
	if (!fp)
		printf("Failed to open file"); // Could not open file.

	// Checks that the WAVE file is OK.
	fread(type, sizeof(char), 4, fp); // Reads the first bytes in the file.
	if (type[0] != 'R' || type[1] != 'I' || type[2] != 'F' || type[3] != 'F') // Should be "RIFF".
		printf("Not RIFF"); // Not RIFF

	fread(&size, sizeof(DWORD), 1, fp); // Continue to read the file.
	fread(type, sizeof(char), 4, fp); // Continue to read the file.
	if (type[0] != 'W' || type[1] != 'A' || type[2] != 'V' || type[3] != 'E') // This part should be "WAVE".
		printf("Not WAVE"); //Not WAVE.

	fread(type, sizeof(char), 4, fp); // Continue to read the file.
	if (type[0] != 'f' || type[1] != 'm' || type[2] != 't' || type[3] != ' ') // This part should be "fmt ".
		printf("Not fmt "); //Not fmt.

	fread(&chunkSize, sizeof(DWORD), 1, fp);
	fread(&formatType, sizeof(short), 1, fp);
	fread(&channels, sizeof(short), 1, fp);
	fread(&sampleRate, sizeof(DWORD), 1, fp);
	fread(&avgBytesPerSec, sizeof(DWORD), 1, fp);
	fread(&bytesPerSample, sizeof(short), 1, fp);
	fread(&bitsPerSample, sizeof(short), 1, fp);

	fread(type, sizeof(char), 4, fp);
	if (type[0] != 'd' || type[1] != 'a' || type[2] != 't' || type[3] != 'a') // This part should be "data".
		printf("Missing DATA");

	fread(&dataSize, sizeof(DWORD), 1, fp);

	// Displays the info about the WAVE file.
	//std::cout << "Chunk Size: " << chunkSize << "\n";
	//std::cout << "Format Type: " << formatType << "\n";
	//std::cout << "Channels: " << channels << "\n";
	//std::cout << "Sample Rate: " << sampleRate << "\n";
	//std::cout << "Average Bytes Per Second: " << avgBytesPerSec << "\n";
	//std::cout << "Bytes Per Sample: " << bytesPerSample << "\n";
	//std::cout << "Bits Per Sample: " << bitsPerSample << "\n";
	//std::cout << "Data Size: " << dataSize << "\n";

	buf = new unsigned char[dataSize];
	//std::cout << fread(buf, sizeof(BYTE), dataSize, fp) << " bytes loaded\n";
	fread(buf, sizeof(BYTE), dataSize, fp);

	frequency = sampleRate;

	if (bitsPerSample == 8)
	{
		if (channels == 1)
			format = AL_FORMAT_MONO8;
		else if (channels == 2)
			format = AL_FORMAT_STEREO8;
	}
	else if (bitsPerSample == 16)
	{
		if (channels == 1)
			format = AL_FORMAT_MONO16;
		else if (channels == 2)
			format = AL_FORMAT_STEREO16;
	}
	if (!format)
		printf("Wrong BitPerSample");

	switch (fnum)
	{
	case 1:
		alBufferData(buffer1, format, buf, dataSize, frequency);
		break;
	case 2:
		alBufferData(buffer2, format, buf, dataSize, frequency);
		break;
	case 3:
		alBufferData(buffer3, format, buf, dataSize, frequency);
		break;
	default:
		break;
	}
	if (alGetError() != AL_NO_ERROR)
		printf("Error loading ALBuffer");

	// Listener.                                                                            
	alListenerfv(AL_POSITION, ListenerPos);		// Set position of the listener.
	alListenerfv(AL_VELOCITY, ListenerVel);		// Set velocity of the listener.
	alListenerfv(AL_ORIENTATION, ListenerOri);	// Set orientation of the listener.

	// Source.
	switch (fnum)
	{
	case 1:
		alSourcei(source1, AL_BUFFER, buffer1);			// Link the buffer to the source.
		alSourcef(source1, AL_PITCH, 1.0f);				// Set the pitch of the source.
		alSourcef(source1, AL_GAIN, 1.0f);				// Set the gain of the source.
		alSourcefv(source1, AL_POSITION, SourcePos);	// Set the position of the source.
		alSourcefv(source1, AL_VELOCITY, SourceVel);	// Set the velocity of the source.
		alSourcei(source1, AL_LOOPING, AL_FALSE);		// Set if source is looping sound.
		break;
	case 2:
		alSourcei(source2, AL_BUFFER, buffer2);			// Link the buffer to the source.
		alSourcef(source2, AL_PITCH, 1.0f);				// Set the pitch of the source.
		alSourcef(source2, AL_GAIN, 1.0f);				// Set the gain of the source.
		alSourcefv(source2, AL_POSITION, SourcePos);	// Set the position of the source.
		alSourcefv(source2, AL_VELOCITY, SourceVel);	// Set the velocity of the source.
		alSourcei(source2, AL_LOOPING, AL_FALSE);		// Set if source is looping sound.
		break;
	case 3:
		alSourcei(source3, AL_BUFFER, buffer3);			// Link the buffer to the source.
		alSourcef(source3, AL_PITCH, 1.0f);				// Set the pitch of the source.
		alSourcef(source3, AL_GAIN, 1.0f);				// Set the gain of the source.
		alSourcefv(source3, AL_POSITION, SourcePos);	// Set the position of the source.
		alSourcefv(source3, AL_VELOCITY, SourceVel);	// Set the velocity of the source.
		alSourcei(source3, AL_LOOPING, AL_FALSE);		// Set if source is looping sound.
		break;
	default:
		break;
	}
}

void CleanUpSoundStuff(int fnum) // Cleans up some stuff used in the loading and playing of sounds process.
{
	fclose(fp);						// Closes the WAVE file.
	//delete[] buf;					// Deletes the sound data buffer.
	//alDeleteSources(1, &source);	// Deletes the OpenAL Source.
	//alDeleteBuffers(1, &buffer);	// Deletes the OpenAL Buffer.
	//alcMakeContextCurrent(NULL);	// Makes no context current.
	//alcDestroyContext(context);	// Destroys the OpenAL Context.
	//alcCloseDevice(device);		// Closes the OpenAL Device.

	if (fnum == 1)
	{
		wasPlayingSound1 = false;
	}
	else if (fnum == 2)
	{
		wasPlayingSound2 = false;
	}
}

void PrintGameInfo() // Prints some good-to-know information.
{
	std::cout << "/-----Flight simulator control scheme: (all in lower-case)-------\\\n";
	std::cout << "|w - pitch up                                                    |\n";
	std::cout << "|s - pitch down                                                  |\n";
	std::cout << "|a - yaw left                                                    |\n";
	std::cout << "|d - yaw right                                                   |\n";
	std::cout << "|q - roll counterclockwise                                       |\n";
	std::cout << "|e - roll clockwise                                              |\n";
	std::cout << "|--Debug controls: (play without these for more of a challenge)--|\n";
	std::cout << "|r - increase velocity                                           |\n";
	std::cout << "|f - decrease velocity                                           |\n";
	std::cout << "|p - resets the game                                             |\n";
	std::cout << "\\----------------------------------------------------------------/\n";
}

int main(int argc, const char *argv[])
{
	glutInit(&argc, (char**)argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1280, 720);			// Window size.
	glutCreateWindow("Flight simulator");	// Window title.
	glutDisplayFunc(display);
	glutPassiveMotionFunc(CheckMouse);
	init();
	glutTimerFunc(20, &OnTimer, 0);
	glutMainLoop();
}