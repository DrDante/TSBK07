#version 150

in vec3 inPosition;	// Vertex position.
in vec2 inTexCoord;	// Texture coordinate.
in vec3 inNormal;	// Vertex normal.

out vec2 outTexCoord;
out vec3 outNormal;
out vec3 outCamPos;
out vec3 outObjPos;

out float pos;

uniform mat4 MTWMatrix;	// Model-to-world matrix.
uniform mat4 WTVMatrix;	// World-to-view matrix (camera).
uniform mat4 VTPMatrix;	// View-to-projection matrix (lens).
uniform vec3 camPos;	// Camera position.

mat3 normalTransformMatrix = mat3(MTWMatrix);

void main(void)
{
	gl_Position = VTPMatrix * WTVMatrix * MTWMatrix * vec4(inPosition, 1.0);
	outTexCoord = inTexCoord;
	outNormal = normalTransformMatrix * inNormal;
	
	vec3 tempPos = vec3(MTWMatrix * vec4(inPosition, 1.0));
	pos = tempPos.y;

	vec3 temp = vec3(camPos[0], camPos[1], camPos[2]);
	outCamPos = temp;
	outObjPos = vec3(MTWMatrix * vec4(inPosition, 1));
}
