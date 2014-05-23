#version 150

in  vec3 inPosition;
in vec4 inColor;

out vec4 vertexColor;

uniform mat4 MTWMatrix;	// Model-to-world matrix.
uniform mat4 WTVMatrix;	// World-to-view matrix (camera).
uniform mat4 VTPMatrix;	// View-to-projection matrix (lens).

void main(void)
{
		gl_Position = VTPMatrix * WTVMatrix * MTWMatrix * vec4(inPosition, 1.0);
		vertexColor = inColor;
}
