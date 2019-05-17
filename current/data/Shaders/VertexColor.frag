#version 330

in vec2 UV;		// texture coord (interpolated per fragment)
in vec4 vtxCol;
uniform sampler2D ts_diffuse; // texture sampler (passed by application)

out vec4 color;	// final fragment color
void main(){
	// Output color = color of the texture at the specified UV
	color = vtxCol;
}