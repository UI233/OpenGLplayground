#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex;

uniform mat4 transform;
uniform mat4 model;

out vec4 model_pos;
out vec3 model_normal;
out vec2 model_tex;

void main()
{
   vec4 out_pos = transform * model * vec4(pos, 1.0);
   model_pos = model * vec4(pos, 1.0);
   model_pos = vec4(model_pos.xyz / model_pos.w, out_pos.z / out_pos.w);
   model_normal = (transpose(inverse(model)) * vec4(normal, 0.0f)).xyz;
   model_tex = tex;
   gl_Position = out_pos;
}