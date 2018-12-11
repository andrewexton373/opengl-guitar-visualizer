#version 330 core 
in vec3 fragNor;
in vec3 pos;
in vec2 fragTex;

uniform sampler2D tex;

out vec4 color;


void main() {
	
	color = texture(tex, fragTex);
	color.a=1;
	 
}
