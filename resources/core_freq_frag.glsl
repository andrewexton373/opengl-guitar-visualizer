#version 330 core
in vec3 fragNor;
out vec4 color;
in vec3 pos;
in vec2 fragTex;

uniform float time;
uniform sampler2D tex;


void main() {
    
    color = (vec4(sin(time) + pos.x, cos(time) + pos.y, 1 - sin(time) + pos.z, 1)) + 0.5;

}
