#version 330 core
in vec3 fragNor;
out vec4 color;
in vec3 pos;
in vec2 fragTex;

uniform sampler2D tex;

void main() {
    color = (vec4(pos.x, pos.y, pos.z, 1)*(1 - pos.z)) + 0.5;
    color.a = 1.0;
}
