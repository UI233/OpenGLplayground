#version 450 core
uniform sampler2D tex;

layout (location = 0) out vec4 pos;
layout (location = 1) out vec4 color;
layout (location = 2) out vec3 normal;

in vec4 model_pos;
in vec3 model_normal;
in vec4 model_color;
in vec2 model_tex;

void main()
{
    pos = model_pos;
    color = texture(tex, model_tex);
    if(color.xyz == vec3(0.0))
        color = vec4(1.0);
    normal = normalize(model_normal);
}