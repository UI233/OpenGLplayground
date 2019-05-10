#version 450 core
layout(location = 0) in vec3 frag_pos;

uniform sampler2D real_pos;
uniform sampler2D normal;
uniform sampler2D color;
uniform sampler2D depth;
out vec2 tex;
uniform mat4 light_transform;
void main()
{
    tex = 0.5 * frag_pos.xy + vec2(0.5, 0.5);
    gl_Position = vec4(frag_pos , 1.0);
}