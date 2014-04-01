#version 150

in vec2 outTexCoord;
in vec3 outNormal;
out vec4 out_Color;

uniform sampler2D texUnit;

void main(void)
{
	const vec3 light = vec3(3.0, 3.0, 3.0);
	out_Color = texture(texUnit, outTexCoord) * dot(light, normalize(outNormal));
}
