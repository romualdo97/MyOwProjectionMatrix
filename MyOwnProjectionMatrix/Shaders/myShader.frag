#version 330 core

// FRAGMENT SHADER INPUT
// from vertex shader
in vec3 vColor;
in vec2 vTexCoord;

// from CPU as uniform
uniform float uTime;
uniform sampler2D uTextureA;
uniform sampler2D uTextureB;

// FRAGMENT SHADER OUTPUT
out vec4 FragColor;

// LOCAL IDENTIFIERS
#define PI 3.141516
vec4 color;

// use the vertex colors?
#define SWAP_TEXTURES

void main()
{
	// The texelA, B of this fragment shader 
	// is then the (filtered) color of the 
	// texture at the (interpolated) texture coordinate.
	
#ifdef SWAP_TEXTURES
	vec4 anim = vec4(0.5 + 0.5 * sin(uTime), 0.5 + 0.5 * sin(uTime + PI), 0.5 + 0.5 * sin(uTime + 0.5 * PI), 1.0);
	vec4 texelA = texture(uTextureA, vTexCoord);
	vec4 texelB = texture(uTextureB, vTexCoord);
	vec4 tint = vec4(vec3(vColor), 1.0) * anim;
	tint = 0.8 + 0.9 * tint;
	color = mix(texelA, texelB, 0.5 + 0.5 * sin(uTime));
	color = color * 0.8;
	FragColor =  color * tint;
#else
	FragColor = vec4(vColor.rgb, 1.0);
#endif
}