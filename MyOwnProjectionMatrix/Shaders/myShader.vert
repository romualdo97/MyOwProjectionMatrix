#version 330 core

// VERTEX SHADER INPUT: 

// vertex attributes
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

// uniforms
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

// VERTEX SHADER OUTPUT: pass info from 'vs' to 'fs'
out vec3 vColor;
out vec2 vTexCoord;

// LOCAL IDENTIFIERS
vec4 pos;

void main()
{
	gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);
	vColor = aColor;
	vTexCoord = aTexCoord;
}