/* Handels the steering of the plane */
#include "plane.h"

#define PI 3.14159265358979323846
mat4 planeTransl;
mat4 planeYRotation;
mat4 planeXRotation;
mat4 planeZRotation;
mat4 planeRotation;
mat4 totalTransform;
bool isTurningLeft= FALSE;
bool isTurningRight= FALSE;
bool isPitchingUp= FALSE;
bool isPitchingDown= FALSE;
GLfloat rollAngle=0;

mat4 placingPlane(vec3 l, vec3 p, vec3 s, vec3 v){

	/*	vec3 l point camera is looking a
	vec3 p camera position
	vec3 s forward vec for camera (s = l - p)
	*/

	//Försök att placera planet på ett vettigt sätt framför kameran. 
	// TODO att vinkla planet runt x- och z-axeln när kameran tittar upp/ner
	//      att vinkla planet något när det svänger

	// Find down-vec to plane
	vec3 down = -1*v;
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

	// Check if turning

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
	if (isTurningLeft || isTurningRight){
		if ((rollAngle<(PI/4) && isTurningLeft) || (~isTurningLeft && ~isTurningRight && rollAngle <0)){
			rollAngle = rollAngle - 0.4;
			printf("Hej!");
		}
		if ((rollAngle<-1 * PI / 4 && isTurningRight) || (~isTurningLeft && ~isTurningRight && rollAngle >0)){
			rollAngle = rollAngle  + 1;
		}

	}
	planeZRotation = Rz(rollAngle);
	printf("rollAngle: %f",rollAngle);

	//Total rotation matrix
	planeRotation = Mult(planeYRotation,Mult(planeXRotation,planeZRotation));

	// Make total plane transformation matrix
	totalTransform = Mult(planeTransl, planeRotation);

	return totalTransform ;
}

// Hanterar planets transf-matris när planet svänger (ska vinkla planet lite åt sväng-hållet)
void yawTurn(bool isTurnLeft,bool isTurnRight){
	isTurningLeft = isTurnLeft;
	isTurningRight=isTurnRight; 
}
// Hanterar planets transf-matris när planet pitchar (så den vinklas lite upp/ner, eller ska bara höjdrodrenra ändras?)
void pichTurn(bool isUp, bool isDown){
	isPitchingUp = isUp;
	isPitchingDown = isDown; 
}