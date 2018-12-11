#version 330 core 
in vec3 fragNor;
in vec3 pos;
in vec2 fragTex;

out vec4 color;


void main() {
	
    color = vec4(1, 1, 1, 1);
	color.a=0.8;
	 
}
