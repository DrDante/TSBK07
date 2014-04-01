#version 150

in vec3 inPosition;
in vec2 inTexCoord;
in vec3 inNormal;
out vec2 outTexCoord;
out vec3 outNormal;

uniform mat4 projMatrix;
uniform mat4 mdlMatrix;
uniform mat4 camMatrix;

mat3 normalTransformMatrix = mat3(mdlMatrix);

void main(void)
{
	gl_Position = projMatrix * camMatrix * mdlMatrix * vec4(inPosition, 1.0);
	outTexCoord = inTexCoord;
	outNormal = normalTransformMatrix * inNormal;
}
