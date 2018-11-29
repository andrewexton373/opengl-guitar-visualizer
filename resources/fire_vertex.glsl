#version 330 core
layout(location = 0) in vec3 vPos;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

out float height;

void main()
{
    height = vPos.y;
    gl_Position = P * V * M * vec4(vPos, 1.0);
}

