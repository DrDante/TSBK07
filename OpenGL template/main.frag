#version 150

in vec2 outTexCoord;
in vec3 outNormal;
in vec3 outCamPos;
in vec3 outObjPos;
out vec4 out_Color;

uniform sampler2D texUnit, texUnit2, texUnit3;

in float pos;

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
	// Calculates the incident and reflected light rays for all light sources.
	for (int i = 0; i < numberOfLightSources; i++)
	{
		if(isDirectional[i])
		{
			s[i] = normalize(lightSourcesDirPosArr[i]);
		}
		else
		{
			s[i] = normalize(lightSourcesDirPosArr[i] - outObjPos);
		}
		r[i] = normalize(2 * outNormal * dot(normalize(s[i]), normalize(outNormal)) - s[i]);
	}
	// Calculates the eye vector.
	eye = normalize(outCamPos-outObjPos);

	// Lighting according to the Phong model.
	kdiff = 1.0;	// Needs to be lower for more light sources.
	kspec = 1.0;
	ambLight = kdiff * vec3(0.1, 0.1, 0.1);
	diffLight = vec3(0.0, 0.0, 0.0);
	specLight = vec3(0.0, 0.0, 0.0);
	// Calculates diffuse light.
	for (int i = 0; i < numberOfLightSources; i++)
	{
		diffLight += kdiff * lightSourcesColorArr[i] * max(0.0, dot(s[i], normalize(outNormal)));
	}
	// Calculates specular light, if the surface is not Lambertian.
	if(!lambert)
	{
		for (int i = 0; i < numberOfLightSources; i++)
		{
			specLight += kspec * lightSourcesColorArr[i] * max(0.0, pow(dot(r[i], eye), specularExponent[i]));
		}
	}
	totalLight = vec3(0.0, 0.0, 0.0);
	// Adds the different light components to the total light.
	totalLight += ambLight;
	totalLight += diffLight;
	totalLight += specLight;

	vec4 tempTotalLight = vec4(totalLight, 1.0);
	if(!multitex)
	{
		out_Color = texture(texUnit, outTexCoord) * tempTotalLight;
	}
	else
	{
		float zone1end = 10.0;
		float zone2start = 18.0;
		float zone2end = 25.0;
		float zone3start = 60.0;
		vec4 tempOutColor = vec4(0.0, 0.0, 0.0, 0.0);
		if(pos < zone1end)
			out_Color = texture(texUnit, outTexCoord) * tempTotalLight;
		else if(pos >= zone1end && pos < zone2start)
		{
			tempOutColor += texture(texUnit, outTexCoord) * (zone2start - pos) / (zone2start - zone1end);
			tempOutColor += texture(texUnit2, outTexCoord) * (pos - zone1end) / (zone2start - zone1end);
			out_Color = tempOutColor * tempTotalLight;
		}
		else if(pos >= zone2start && pos < zone2end)
			out_Color = texture(texUnit2, outTexCoord) * tempTotalLight;
		else if(pos >= zone2end && pos < zone3start)
		{
			tempOutColor += texture(texUnit2, outTexCoord) * (zone3start - pos) / (zone3start - zone2end);
			tempOutColor += texture(texUnit3, outTexCoord) * (pos - zone2end) / (zone3start - zone2end);
			out_Color = tempOutColor * tempTotalLight;
		}
		else
			out_Color = texture(texUnit3, outTexCoord) * tempTotalLight;
	}
}
