// Includes
#include <gl\glew.h>
#include <gl\freeglut.h>
#include <math.h>
#include "GL_utilities.h"
#include "loadobj.h"
#include "VectorUtils3.h"
#include "LoadTGA.h"


// Globals

// Frustum
#define near 1.0
#define far 30.0
#define right 0.5
#define left -0.5
#define top 0.5
#define bottom -0.5
GLfloat projectionMatrix[] = {	2.0f*near / (right - left), 0.0f, (right + left) / (right - left), 0.0f,
								0.0f, 2.0f*near / (top - bottom), (top + bottom) / (top - bottom), 0.0f,
								0.0f, 0.0f, -(far + near) / (far - near), -2 * far*near / (far - near),
								0.0f, 0.0f, -1.0f, 0.0f };

Model *car;
Model *bunny;

// Time variable
GLfloat t;

// Rotation, translation and result matrices
mat4 rot, trans, camera, total;
mat4 rot2, trans2, total2;
mat4 rot3, trans3, total3;

// Camera variables
GLfloat px, py, pz, lx, ly, lz, vx, vy, vz;

// Reference to shader program
GLuint program;

// Reference to texture
GLuint myTex;
GLuint myTex2;

GLenum err;

void init(void)
{
	// Initializing GLEW
	err = glewInit();

	dumpInfo();

	// Loading models
	car = LoadModelPlus("bilskiss.obj");
	bunny = LoadModelPlus("bunnyplus.obj");

	// GL inits
	glClearColor(0.0, 0.3, 0.3, 0);
	glEnable(GL_DEPTH_TEST);
	printError("GL inits");

	// Load and compile shaders
	program = loadShaders("main.vert", "main.frag");
	printError("init shader");

	// Load textures
	LoadTGATextureSimple("bilskissred.tga", &myTex);
	LoadTGATextureSimple("maskros512.tga", &myTex2);

	printError("init arrays");
}

void display(void)
{
	printError("pre display");

	// Update t for the current elapsed time
	t = (GLfloat)glutGet(GLUT_ELAPSED_TIME);

	// Moving the car
	trans = T(5.0, 5, 5);
	rot = Ry(0.003 * t);
	total = Mult(trans, rot);

	// Moving the bunny
	trans2 = T(5.0, 0.0, 0.0);
	rot2 = Ry(0.001 * t);
	total2 = Mult(rot2, trans2);

	// Moving origin bunny
	trans3 = T(0.0, 0.0, 0.0);
	rot3 = Ry(0);
	total3 = Mult(rot3, trans3);

	// Setting camera coordinates, directions and up-vector
	px = 15.0 * cosf(0.0005 * t);
	py = 15.0;
	pz = 15.0 * sinf(0.0005 * t);
	lx = -15.0 * cosf(0.0005 * t);
	ly = -15.0;
	lz = -15.0 * sinf(0.0005 * t);
	vx = 0.0;
	vy = 1.0;
	vz = 0.0;
	camera = lookAt(px, py, pz, lx, ly, lz, vx, vy, vz);

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Active texture pbject
	glBindTexture(GL_TEXTURE_2D, myTex);
	DrawModel(car, program, "inPosition", "inNormal", "inTexCoord");

	// Upload the affine result matrix
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	// Upload projection matrix	
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix);
	// Upload camera matrix
	glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, camera.m);

	// Upload texture unit
	glUniform1i(glGetUniformLocation(program, "texUnit"), 0); // Texture unit 0

	//Active texture pbject
	glBindTexture(GL_TEXTURE_2D, myTex2);
	DrawModel(bunny, program, "inPosition", "inNormal", "inTexCoord");

	// Upload the affine result matrix
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total2.m);
	// Upload projection matrix	
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix);
	// Upload camera matrix
	glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, camera.m);

	// Upload texture unit
	glUniform1i(glGetUniformLocation(program, "texUnit"), 0); // Texture unit 0

	//Active texture pbject
	glBindTexture(GL_TEXTURE_2D, myTex2);
	DrawModel(bunny, program, "inPosition", "inNormal", "inTexCoord");

	// Upload the affine result matrix
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total3.m);
	// Upload projection matrix	
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix);
	// Upload camera matrix
	glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, camera.m);

	// Upload texture unit
	glUniform1i(glGetUniformLocation(program, "texUnit"), 0); // Texture unit 0

	printError("display");

	//glFlush();
	glutSwapBuffers();
}

void OnTimer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(20, &OnTimer, value);
}

int main(int argc, const char *argv[])
{
	glutInit(&argc, (char**)argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("GL3 template");
	glutDisplayFunc(display);
	init();
	glutTimerFunc(20, &OnTimer, 0);
	glutMainLoop();
}
