// Includes.
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

#include "particle.h"




// ---Globals---
#define PI 3.14159265358979323846

// Frustum.
#define near 1.0
#define far 3000.0	// Drawing distance.
#define right 0.5
#define left -0.5
#define top 0.5
#define bottom -0.5
GLfloat projMatrix[] = { 2.0f*near / (right - left), 0.0f, (right + left) / (right - left), 0.0f,
0.0f, 2.0f*near / (top - bottom), (top + bottom) / (top - bottom), 0.0f,
0.0f, 0.0f, -(far + near) / (far - near), -2 * far*near / (far - near),
0.0f, 0.0f, -1.0f, 0.0f }; // Projection matrix.

// Time variable.
GLfloat t;

// Camera matrices.
mat4 camMatrix;
GLfloat skyMatrix[16];

// Camera variables.
vec3 p = vec3(10.0, 25.0, 20.0);	// Camera placement.
vec3 l = vec3(10.0, 25.0, 0.0);	// Point the camera is looking at.
vec3 v = vec3(0.0, 1.0, 0.0);	// "Up" vector for the camera.
vec3 s = l - p;					// "Forward" vector for the camera.

// Reference to shader program.
GLuint program;

// Function declarations.
void UploadAndDraw(GLfloat totalMat[], Model *currentModel, bool isSkybox, bool isLambertian);
void CheckMouse(int x, int y);
void SetCameraVector(float fi, float theta);
void CheckKeys();

vec3 ConfinedUpVector(vec3 forward);
mat4 RotatePlaneModel();
vec3 CameraPlacement(vec3 stiffPos, vec3 upVec, vec3 rightVec);
void InitAfterCrash();

// Hitta rätt plats för!
bool CheckCollisionWithGround(GLfloat x, GLfloat y, GLfloat z);
bool isExplosion = FALSE;
GLint count=0;

// Glew initialization... thing.
GLenum err;



// -------------------LIGHT SOURCE(S)-------------------
const int numberOfLightSources = 4;	// Change here and in the fragment shader when setting the number of light sources.
Point3D lightSourcesColorsArr[] = { { 1.0f, 1.0f, 1.0f }, // White light
{ 0.0f, 0.0f, 0.0f }, // No light
{ 0.0f, 0.0f, 0.0f }, // No light
{ 0.0f, 0.0f, 0.0f } }; // No light
GLfloat specularExponent[] = { 280.0, 0.0, 0.0, 0.0 };
GLint isDirectional[] = { 1, 0, 0, 0 };
Point3D lightSourcesDirectionsPositions[] = { { -0.58f, 0.58f, -0.58f }, // White light, directional
{ 0.0f, 0.0f, 0.0f }, // No light
{ 0.0f, 0.0f, 0.0f }, // No light
{ 0.0f, 0.0f, 0.0f } }; // No light
// -----------------------------------------------------
// ----------------------OBJECT(S)----------------------
// Models.
Model *windmillWalls;
Model *windmillRoof;
Model *windmillBalcony;
Model *windmillBlade;
Model *ground;
Model *skybox;
Model *bunny;
Model *teapot;
Model *car;
Model *teddy;
Model *terrain;
Model *plane;
Model *planeRot;
Model *trunk;
Model *leaves;

// Rotation, translation and result matrices for the models.
mat4 bunnyTrans, teapotTrans, carTrans, teddyTrans;
mat4 teapotScale, teapotTotal;
mat4 statTrans, statTotal;
mat4 wallsRot, wallsTrans;
mat4 roofRot, roofTrans;
mat4 balconyRot, balconyTrans;
mat4 bladeRot, bladeStartRot, bladeTrans;
mat4 bladeTotal1, bladeTotal2, bladeTotal3, bladeTotal4;

mat4 planeTrans;
mat4 planeTotal;

// Plane speed
GLfloat planeSpeed= 1.0;

// References to textures.
GLuint groundTex;
GLuint millTex;
GLuint skyTex;
GLuint bunnyTex, teapotTex, carTex, teddyTex;
GLuint trunkTex, leafTex;

// terrain
TextureData ttex;

int terrainW;
int terrainH;

// -----------------------------------------------------
// -------------

Plane player(vec3(0.0, 20.0, 0.0), vec3(1.0, 0.0, 0.0), 0.5);

tree* treeArray;
cloud* cloudArray;

particle* particleArray;

int nrOfParticles = 50;
bool collisionFirstLoop = TRUE;

void init(void)
{
	err = glewInit();

	dumpInfo();

	// GL inits.
	glClearColor(0.0, 0.3, 0.3, 0);
	glEnable(GL_DEPTH_TEST);
	printError("GL inits");

	// Loading and compiling shaders.
	program = loadShaders("main.vert", "main.frag");
	printError("init shader");

	// ----------------------OBJECT(S)----------------------

	// Load terrain data
	LoadTGATextureData("terrain/arnoldterrang.tga", &ttex);

	terrain = GenerateTerrain(&ttex);
	terrainW = getWidth(&ttex);
	terrainH = getHeight(&ttex);
	printError("init terrain");

	// Loading models.
	windmillWalls = LoadModelPlus("models/windmill/windmill-walls.obj");
	windmillRoof = LoadModelPlus("models/windmill/windmill-roof.obj");
	windmillBalcony = LoadModelPlus("models/windmill/windmill-balcony.obj");
	windmillBlade = LoadModelPlus("models/windmill/blade.obj");
	ground = LoadModelPlus("models/ground.obj");
	skybox = LoadModelPlus("models/skybox.obj");
	bunny = LoadModelPlus("models/bunnyplus.obj");
	teapot = LoadModelPlus("models/teapot.obj");
	car = LoadModelPlus("models/bilskiss.obj");
	teddy = LoadModelPlus("models/teddy.obj");
	plane = LoadModelPlus("models/LPNoBladeobj.obj");
	planeRot = LoadModelPlus("models/Blade.obj");
	trunk = LoadModelPlus("models/stamm.obj");
	leaves = LoadModelPlus("models/blad.obj");


	// Loading textures.
	LoadTGATextureSimple("textures/grass.tga", &groundTex);
	LoadTGATextureSimple("textures/conc.tga", &millTex);
	LoadTGATextureSimple("textures/Skybox512.tga", &skyTex);
	LoadTGATextureSimple("textures/dirt.tga", &bunnyTex);
	LoadTGATextureSimple("textures/rutor.tga", &teapotTex);
	LoadTGATextureSimple("textures/bilskissred.tga", &carTex);
	LoadTGATextureSimple("textures/maskros512.tga", &teddyTex);
	LoadTGATextureSimple("textures/treetext2.tga", &trunkTex);
	LoadTGATextureSimple("textures/leaftext2.tga", &leafTex);
	// -----------------------------------------------------

	// Multitexturing.
	glUniform1i(glGetUniformLocation(program, "texUnit"), 0);
	glUniform1i(glGetUniformLocation(program, "texUnit2"), 1);

	// Initializing "keyboard".
	initKeymapManager();

	// "Initializing" camera.
	s = Normalize(s);
	camMatrix = lookAtv(p, l, v);

	// Initialize forest.
	treeArray = GetForest(terrain->vertexArray, terrainW, terrainH, 30);
	cloudArray = GetClouds(terrain->vertexArray, terrainW, terrainH, 60);

	//"Particles"
	particleArray = GenerateParticles(nrOfParticles);

	//glEnable(GL_CULL_FACE);
	//glDisable(GL_CULL_FACE);
}

void display(void)
{
	printError("pre display");

	// Update t for the current elapsed time.
	t = (GLfloat)glutGet(GLUT_ELAPSED_TIME);

	CheckKeys();

	// Clear the screen.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);

	// -----------------------SKYBOX------------------------
	// Skybox.
	glBindTexture(GL_TEXTURE_2D, skyTex);
	UploadAndDraw(skyMatrix, skybox, 1, 0);
	// -----------------------------------------------------

	// Lighting stuff.
	glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), numberOfLightSources, &lightSourcesDirectionsPositions[0].x);
	glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), numberOfLightSources, &lightSourcesColorsArr[0].x);
	glUniform1fv(glGetUniformLocation(program, "specularExponent"), numberOfLightSources, specularExponent);
	glUniform1iv(glGetUniformLocation(program, "isDirectional"), numberOfLightSources, isDirectional);

	// Uploading the camera position (in world coordinates) to the shaders.
	GLfloat camPos[3] = { p.x, p.y, p.z };
	glUniform3fv(glGetUniformLocation(program, "camPos"), 1, camPos);

	// ----------------------OBJECT(S)----------------------
	// Translation matrices.
	statTrans = T(0.0, 0.0, 0.0);		// Placement of the static pieces.
	bladeTrans = T(4.5, 9.2, 0.0);		// Placement of the blades.
	bladeRot = Rx(0.001 * t);			// Blade rotation speed.
	statTotal = statTrans;				// In this case, no rotation is used.

	// Ny terräng
	glBindTexture(GL_TEXTURE_2D, groundTex);
	UploadAndDraw(statTotal.m, terrain, 0, 0);
	/* Ta bort denna kommentar o kommentera ut koden nedan om du vill styra runt utan planet!
	// Planet som det var förut
	planeTrans = T(15.0, 15.0, 0.0);
	mat4 temp0 = Ry(-PI*0.5);
	mat4 temp = Ry(0.0001*t);
	temp = Mult(temp, temp0);
	//planeTotal = (planeTrans, temp);
	planeTotal = Mult(temp, planeTrans);

	// Plane
	glBindTexture(GL_TEXTURE_2D, skyTex);
	UploadAndDraw(planeTotal.m, plane, 0, 0);
	mat4 temp2 = Rz(0.03*t);
	planeTotal = Mult(planeTrans, temp2);
	planeTotal = Mult(temp, planeTotal);
	UploadAndDraw(planeTotal.m, planeRot, 0, 0);
	*/
	
	// *** NEW PLANE CODE ***
	player.MovePlane();

	// Check if collided with ground
	if (CheckCollisionWithGround(player.GetPosition().x, player.GetPosition().y, player.GetPosition().z)){
		isExplosion = TRUE;
		player.SetCollision(TRUE);
	}

	// Rotating model.
	mat4 PlaneMatrix = RotatePlaneModel();
	// Moving model.
	PlaneMatrix = Mult(T(player.GetPosition().x, player.GetPosition().y, player.GetPosition().z), PlaneMatrix);

	glBindTexture(GL_TEXTURE_2D, skyTex);
	UploadAndDraw(PlaneMatrix.m, plane, 0, 0);

	// Camera stuff.
	s = player.GetDirection(); // Forward vector.
	l = player.GetPosition(); // What the camera is looking at.
	p = l - s * 20.0; // Stiff camera placement, 20.0 behind the plane.
	vec3 sluggishCamPos = l + CameraPlacement(p, player.GetUpVector(), Normalize(CrossProduct(player.GetDirection(), player.GetUpVector())));

	// OBS! Subjektivt, bör diskuteras.
	v = ConfinedUpVector(player.GetDirection());
	//camMatrix = lookAtv(p, l, player.GetUpVector());
	//camMatrix = lookAtv(p, l, v);
	camMatrix = lookAtv(sluggishCamPos, l, v);



	// Blades
	mat4 temp2 = Rz(0.03*t);
	planeTotal = Mult(PlaneMatrix, temp2);
	UploadAndDraw(planeTotal.m, planeRot, 0, 0);

	// *** END PLANE CODE ***

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

		if (treeArray[i].CheckHitBox(player.GetPosition())) // Check player collision with tree
		{
			isExplosion = TRUE;
			player.SetCollision(TRUE);

			if (collisionFirstLoop)
			{
				for (int i = 0; i < nrOfParticles; i++)
				{
					particleArray[i].SetParticleOrigin(player.GetPosition());
				}
			}
			collisionFirstLoop = FALSE;
		}
	}


	// Windmill.
	glBindTexture(GL_TEXTURE_2D, millTex);
	// Walls, roof and balcony.
	UploadAndDraw(statTotal.m, windmillWalls, 0, 0);
	UploadAndDraw(statTotal.m, windmillRoof, 0, 0);
	UploadAndDraw(statTotal.m, windmillBalcony, 0, 0);
	// Blades.
	bladeStartRot = Rx(0);
	bladeTotal1 = Mult(bladeRot, bladeStartRot);
	bladeTotal1 = Mult(bladeTrans, bladeTotal1);
	bladeStartRot = Rx(3.14 / 2);
	bladeTotal2 = Mult(bladeRot, bladeStartRot);
	bladeTotal2 = Mult(bladeTrans, bladeTotal2);
	bladeStartRot = Rx(3.14);
	bladeTotal3 = Mult(bladeRot, bladeStartRot);
	bladeTotal3 = Mult(bladeTrans, bladeTotal3);
	bladeStartRot = Rx(3 * 3.14 / 2);
	bladeTotal4 = Mult(bladeRot, bladeStartRot);
	bladeTotal4 = Mult(bladeTrans, bladeTotal4);
	// Upload matrices, blade 1.
	UploadAndDraw(bladeTotal1.m, windmillBlade, 0, 0);
	// Upload matrices, blade 2.
	UploadAndDraw(bladeTotal2.m, windmillBlade, 0, 0);
	// Upload matrices, blade 3.
	UploadAndDraw(bladeTotal3.m, windmillBlade, 0, 0);
	// Upload matrices, blade 4.
	UploadAndDraw(bladeTotal4.m, windmillBlade, 0, 0);

	// "explosion"
	if (isExplosion){

		for (int i = 0; i < nrOfParticles; i++)
		{	
		teapotTrans = T(particleArray[i].GetPosition().x, particleArray[i].GetPosition().y - 2, particleArray[i].GetPosition().z);
		printf("%f \n", particleArray[i].GetPosition().x);
		particleArray[i].UpdateParticle();
		//teapotScale = S(0.2, 0.2, 0.2);
		teapotTotal = teapotTrans;
		UploadAndDraw(teapotTrans.m, teapot, 0, 0);
		}

		count += 1;
		if (count > 80){
			InitAfterCrash();
			count = 0;
		}
	}

	// Extra objects.
	bunnyTrans = T(-20.0, 0.55, -20.0);
	// Testing height function.
	float tempx = 100 + 50 * cosf(0.0005 * t);
	float tempz = 100 + 50 * sinf(0.0005 * t);
	teapotTrans = T(tempx, findHeight(tempx, tempz, terrain->vertexArray, terrainW, terrainH), tempz);
	carTrans = T(20.0, 0.0, -20.0);
	teddyTrans = T(20.0, 1.1, 20.0);
	glBindTexture(GL_TEXTURE_2D, bunnyTex);
	UploadAndDraw(bunnyTrans.m, bunny, 0, 0);
	// ---Multi textured teapot---
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, teapotTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bunnyTex);
	glUniform1i(glGetUniformLocation(program, "multitex"), 1);
	UploadAndDraw(teapotTrans.m, teapot, 0, 0);
	glUniform1i(glGetUniformLocation(program, "multitex"), 0);

	glActiveTexture(GL_TEXTURE0);
	// ---------------------------
	// ---Transparent objects---
	glUniform1i(glGetUniformLocation(program, "transparent"), 1);
	glBindTexture(GL_TEXTURE_2D, carTex);
	UploadAndDraw(carTrans.m, car, 0, 0);
	glBindTexture(GL_TEXTURE_2D, teddyTex);
	UploadAndDraw(teddyTrans.m, teddy, 0, 0);
	glUniform1i(glGetUniformLocation(program, "transparent"), 0);
	// -------------------------
	// -----------------------------------------------------

	printError("display");

	glutSwapBuffers();
}

void OnTimer(int value)
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

		glUniform1i(glGetUniformLocation(program, "skybox"), 1);
		glUniformMatrix4fv(glGetUniformLocation(program, "MTWMatrix"), 1, GL_TRUE, IdentityMatrix().m);
		glUniformMatrix4fv(glGetUniformLocation(program, "WTVMatrix"), 1, GL_TRUE, skyMatrix);
	}
	else
	{
		glUniformMatrix4fv(glGetUniformLocation(program, "MTWMatrix"), 1, GL_TRUE, totalMat);
		glUniformMatrix4fv(glGetUniformLocation(program, "WTVMatrix"), 1, GL_TRUE, camMatrix.m);
	}
	if (isLambertian)
	{
		glUniform1i(glGetUniformLocation(program, "lambert"), 1);	// Disables specular lighting.
	}
	// Drawing the model.
	DrawModel(currentModel, program, "inPosition", "inNormal", "inTexCoord");
	if (isLambertian)
	{
		glUniform1i(glGetUniformLocation(program, "lambert"), 0);	// Re-enables specular lighting.
	}
	if (isSkybox)
	{
		// Restoring some changes made when the skybox was drawn.
		glEnable(GL_DEPTH_TEST);
		glUniform1i(glGetUniformLocation(program, "skybox"), 0);
	}
}

void CheckMouse(int x, int y)	// Aligns camera direction after mouse cursor location.
{
	float xSensMultiplier = 2.0;
	float fi = 2 * PI * (float)x / glutGet(GLUT_WINDOW_WIDTH);
	float theta = PI * (float)y / glutGet(GLUT_WINDOW_HEIGHT);
	//SetCameraVector(xSensMultiplier * (PI - fi), -theta);
}

void SetCameraVector(float fi, float theta)	// Sets the camera matrix.
{
	// Sets s, the direction you're looking at.
	s.z = sinf(theta) * cosf(fi);
	s.x = sinf(theta) * sinf(fi);
	s.y = cosf(theta);
	// Translates this into l, the point "just in front of your face" when looking along s.
	l = p + s;
	camMatrix = lookAtv(p, l, v);
}

bool wasRollingRight = false;
bool wasRollingLeft = false;


float yawCamOffset = 0.0;
float pitchCamOffset = 0.0;
const float pitchSpeed = 0.02;
const float yawSpeed = 0.01;
const float rollSpeed = 0.04;
const float returnSpeed = 0.02;
const float camSpeed = 0.01; // Do not change.
const float camReturnSpeed = 0.01; // Do not change.
const float pitchCamLimit = 0.2; // Do not change.
const float yawCamLimit = 0.2; // Do not change.

void CheckKeys()	// Checks if keys are being pressed.
{
	vec3 tempRight = Normalize(CrossProduct(player.GetDirection(), player.GetUpVector()));
	// 'w' pitches the plane downwards.
	if (keyIsDown('w'))
	{
		vec3 tempForward = Normalize(player.GetDirection() - pitchSpeed * player.GetUpVector());
		vec3 tempUp = Normalize(player.GetUpVector() + pitchSpeed * player.GetDirection());
		player.SetDirection(tempForward, tempUp);
		if (pitchCamOffset < pitchCamLimit)
		{
			pitchCamOffset += camSpeed;
		}
	}
	// 's' pitches the plane upwards.
	if (keyIsDown('s'))
	{
		vec3 tempForward = Normalize(player.GetDirection() + pitchSpeed * player.GetUpVector());
		vec3 tempUp = Normalize(player.GetUpVector() - pitchSpeed * player.GetDirection());
		player.SetDirection(tempForward, tempUp);
		if (pitchCamOffset > -pitchCamLimit)
		{
			pitchCamOffset -= camSpeed;
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
	}
	// 'd' yaws the plane to the right.
	if (keyIsDown('d'))
	{
		player.SetDirection(Normalize(player.GetDirection() + yawSpeed * tempRight), player.GetUpVector());
		if (yawCamOffset < yawCamLimit)
		{
			yawCamOffset += camSpeed;
		}
	}
	// 'q' rolls the plane counterclockwise.
	if (keyIsDown('q'))
	{
		player.SetDirection(player.GetDirection(), Normalize(player.GetUpVector() - rollSpeed * tempRight));
		wasRollingRight = false;
	}
	// 'e' rolls the plane clockwise.
	if (keyIsDown('e'))
	{
		player.SetDirection(player.GetDirection(), Normalize(player.GetUpVector() + rollSpeed * tempRight));
		wasRollingLeft = false;
	}
	// 'r' increases the speed.
	if (keyIsDown('r'))
	{
		player.SetVelocity(player.GetVelocity() + 0.01);
	}
	// 'f' decreases the speed.
	if (keyIsDown('f'))
	{
		player.SetVelocity(player.GetVelocity() - 0.01);
	}
	// Slowly resets the roll.
	vec3 yUp = vec3(0.0, 1.0, 0.0);
	if (!keyIsDown('q') && !keyIsDown('e')/* && !keyIsDown('w') && !keyIsDown('s')*/)
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
	}
	// Slowly resets the pitch camera offset.
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
	}
	// Slowly resets the yaw camera offset.
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
	}
}

/* Check if collision with groud given object (x,y,z), 
returns TRUE if collision */
bool CheckCollisionWithGround(GLfloat x, GLfloat y, GLfloat z){
	bool isCollision = FALSE;
	GLfloat groundHeight = findHeight(x, z, terrain->vertexArray, terrainW, terrainH);
	if (isnan(groundHeight) == 1){ // is outside terrain? (Should not happen in finished game!)
		groundHeight = 0;
	}
	if (groundHeight > y){
		isCollision = TRUE;
	}
	return isCollision;
}

vec3 ConfinedUpVector(vec3 forward) // Returns an "up" vector confined to the plane spanned by the forward vector and the y axis.
{
	vec3 yUp = { 0.0, 1.0, 0.0 };
	return Normalize(yUp - DotProduct(yUp, forward) * forward);
}

mat4 RotatePlaneModel()
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

vec3 CameraPlacement(vec3 stiffPos, vec3 upVec, vec3 rightVec)
{
	vec3 planeToCam = stiffPos - player.GetPosition();
	mat4 offsetPitch = ArbRotate(rightVec, pitchCamOffset);
	mat4 offsetYaw = ArbRotate(upVec, yawCamOffset);
	mat4 camResult = Mult(offsetYaw, offsetPitch);
	return vec3(camResult*planeToCam);
}

void InitAfterCrash(){
	player.SetCollision(FALSE);
	player.SetDirection(vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));
	player.SetPosition(vec3(0.0, 20.0, 0.0));
	player.SetVelocity(0.5);
	isExplosion = FALSE;
	collisionFirstLoop = TRUE;
}


int main(int argc, const char *argv[])
{
	glutInit(&argc, (char**)argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 600);	// Window size.
	glutCreateWindow("OpenGL template");	// Window title.
	glutDisplayFunc(display);
	glutPassiveMotionFunc(CheckMouse);
	init();
	glutTimerFunc(20, &OnTimer, 0);
	glutMainLoop();
}