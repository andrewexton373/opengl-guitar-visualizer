#version 330 core
out vec4 color;
in float height;

void main()
{
    color.rgb = vec3(1,1,1);
    color.a = 1;
//    color.a = 1 - (height * height);
}
