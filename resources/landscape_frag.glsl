#version 330 core
in vec3 fragNor;
out vec4 color;
in vec3 pos;
in vec2 fragTex;

uniform sampler2D tex;
uniform sampler2D tex2;

void main() {
    
    vec3 normal = normalize(fragNor);
    
    vec3 lp = vec3(50, 100, 0);
    vec3 ld = normalize(lp - pos);
    float light = dot(ld,normal);
    
    vec3 cd, rd;
    
    cd = normalize(vec3(0, -400, 0) - pos);
    rd = ld - 2*dot(ld, normal) * normal;
    vec3 r = normalize(rd);
    float spec = dot(cd, r);
    if (spec < 0) spec = 0;
    spec = pow(spec, 60);
    
    
    color = texture(tex2, fragTex)*spec;
    color.a=0.90;
    
    
}
