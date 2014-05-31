#version 150

in vec2 outTexCoord;
in vec3 outNormal;
in vec3 outCamPos;
in vec3 outObjPos;
out vec4 out_Color;

uniform sampler2D texUnit, texUnit2;
uniform bool skybox;
uniform bool lambert;
uniform bool multitex;
uniform bool transparent;
uniform mat4 camMatrix;
// -------------------LIGHT SOURCE(S)-------------------
// Light sources
const int numberOfLightSources = 4;

uniform vec3 lightSourcesDirPosArr[numberOfLightSources];
uniform vec3 lightSourcesColorArr[numberOfLightSources];
uniform float specularExponent[numberOfLightSources];
uniform bool isDirectional[numberOfLightSources];

vec3 r[numberOfLightSources];
vec3 s[numberOfLightSources];	// Incident light.
// -----------------------------------------------------
vec3 eye;	// Vector from the object to the camera.

float kdiff;
float kspec;
vec3 ambLight;		// Ambient light.
vec3 diffLight;		// Diffuse light.
vec3 specLight;		// Specular light.
vec3 totalLight;	// Total light.

void main(void)
{
	out_Color = texture(texUnit, outTexCoord);
}