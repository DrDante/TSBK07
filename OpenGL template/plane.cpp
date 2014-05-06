#include "plane.h"

#define PI 3.14159265358979323846


mat4 placingPlane(vec3 l, vec3 p, vec3 s){

	/*	vec3 l point camera is looking a
		vec3 p camera position
		vec3 s forward vec for camera (s = l - p)
	*/

	//Försök att placera planet på ett vettigt sätt framför kameran. 
	// TODO att vinkla planet runt x- och z-axeln när kameran tittar upp/ner
	//      att vinkla planet något när det svänger

	vec3 planePos = VectorAdd(ScalarMult(Normalize(vec3{ 10 * s.x, -2, 10 * s.z }), 10), p);	// Move plane a bit forward in looking direction and a bit down
	mat4 planeTrans = T(planePos.x, planePos.y, planePos.z); //plane to camera pos

	//Rotation around y-axis 
	vec3 projSVec = { s.x, 0, s.z }; // Project s-vec to xz-plane
	GLfloat angleY = acos((DotProduct(projSVec, vec3{ 0, 0, 1 })) / (Norm(projSVec)*Norm(vec3{ 0, 0, 1 }))); // Rot the plane to lookat-vec
	if (VectorSub(l, p).x>0){
			angleY = -angleY;
		}
	mat4 planeYRotation = Ry(angleY);

	// Rotation around x-axis 
	projSVec = { s.x, 0, s.z };
	GLfloat angleX = acos((DotProduct(projSVec, s)) / (Norm(projSVec)*Norm(s))); // Find angle between forward vec and xz-plane

	if (VectorSub(l, p).y < 0){
		angleX = -angleX;
	}
	mat4 planeXRotation = Rx(-angleX);

	//Total rotation matrix
	mat4 planeRotation = Mult(planeYRotation, planeXRotation);

	// Make total plane transformation matrix
	mat4 totalTransform = Mult(planeTrans, planeRotation);

	return totalTransform ;
}