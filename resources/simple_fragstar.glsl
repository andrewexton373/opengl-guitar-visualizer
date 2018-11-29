#version 330 core 
in vec3 fragNor;
out vec4 color;
in vec3 pos;
in vec2 fragTex;

void main() {
	
    color = vec4(1, 1, 1, 1);
	color.a=0.8;
	 
}
