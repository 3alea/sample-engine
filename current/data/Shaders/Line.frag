#version 330

in vec2 UV;		// texture coord (interpolated per fragment)
in vec4 vtxCol;
out vec4 color;	// final fragment color

void main()
{
	color = vtxCol; // just the color
}