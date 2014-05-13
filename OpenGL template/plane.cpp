/* Handels the steering of the plane */
#include "plane.h"

#define PI 3.14159265358979323846
/*
mat4 planeTransl;
mat4 planeYRotation;
mat4 planeXRotation;
mat4 planeZRotation;
mat4 planeRotation;
mat4 totalTransform;
bool isTurningLeft;
bool isTurningRight;
bool isPitchingUp;
bool isPitchingDown;
bool isReadyToTurn;
GLfloat rollAngle=0;

void initPlane(){
	isTurningLeft = FALSE;
	isTurningRight = FALSE;
	isPitchingUp = FALSE;
	isPitchingDown = FALSE;
}

mat4 placingPlane(vec3 l, vec3 p, vec3 s, vec3 v){

	vec3 down = -1*v; // plane down-vec
	vec3 temp = CrossProduct(s, vec3{ 1, 0, 0 });
	if (temp.x == 0 && temp.y == 0 && temp.z == 0){ // Take the crossprod between forward-vec and ground
		down = Normalize( CrossProduct(s, vec3{ 1, 0, 0 }));
	}
	else{ // Plane parallell to ground
		down = { 0, -1, 0 };
	}
	vec3 planePos = VectorSub(VectorAdd(p, 10*Normalize(s)), -2*down); // Moves plane a bit in front of the cam and down
	planeTransl = T(planePos.x, planePos.y, planePos.z); //plane to camera pos

	//Rotation around y-axis 
	vec3 projSVec = { s.x, 0, s.z }; // Project s-vec to xz-plane
	GLfloat angleY = acos((DotProduct(projSVec, vec3{ 0, 0, 1 })) / (Norm(projSVec)*Norm(vec3{ 0, 0, 1 }))); // Rot the plane to lookat-vec
	if (VectorSub(l, p).x>0){
			angleY = -angleY;
		}
	planeYRotation = Ry(angleY);

	// Rotation around x-axis 
	projSVec = { s.x, 0, s.z };
	GLfloat angleX = acos((DotProduct(projSVec, s)) / (Norm(projSVec)*Norm(s))); // Find angle between forward vec and xz-plane

	if (VectorSub(l, p).y < 0){
		angleX = -angleX;
	}
	planeXRotation = Rx(-angleX);

	// Rotation arounf z-axis
	// Check if turning
	if ((rollAngle<0.8 && isTurningRight) || (!isTurningLeft && !isTurningRight && rollAngle <0)){
		rollAngle = rollAngle + 0.01;
		}
	if ((rollAngle>-1 * 0.8 && isTurningLeft) || (!isTurningLeft && !isTurningRight && rollAngle >0)){
			rollAngle = rollAngle  - 0.01;
		}

	planeZRotation = Rz(rollAngle);

	//Total rotation matrix
	planeRotation = Mult(planeYRotation,Mult(planeXRotation,planeZRotation));

	// Make total plane transformation matrix
	totalTransform = Mult(planeTransl, planeRotation);

	return totalTransform ;
}

// Handels plane-turns
void planeSideTurn(bool isTurnLeft, bool isTurnRight){
	isTurningLeft = isTurnLeft;
	isTurningRight= isTurnRight; 
}

// Returns bool. TRUE if plane ready to turn, FASLE if not
bool planeReadyToTurn(){
	return isReadyToTurn;
}

// Hanterar planets transf-matris när planet pitchar (så den vinklas lite upp/ner, eller ska bara höjdrodrenra ändras?)
void pichTurn(bool isUp, bool isDown){
	isPitchingUp = isUp;
	isPitchingDown = isDown; 
}*/