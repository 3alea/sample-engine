#version 330

layout(location = 0) in vec2 vertexPos;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec4 vertexColor;

out vec2 UV;		// texture coordinates (output to pixel shader)
out vec4 vtxCol;
uniform mat4 mtxModel;	// model view projection
uniform mat4 mtxViewProj;	// model view projection

void main()
{
	// vertex shaders must at least write to gl_Position
	gl_Position =  (mtxViewProj * mtxModel * vec4(vertexPos,0,1)); 

	UV = vertexUV;
	vtxCol = vertexColor;
}