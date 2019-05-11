#version 450 core
#define pi 3.1415926f

uniform sampler2D real_pos;
uniform sampler2D normalmap;
uniform sampler2D color;
uniform sampler2D depth[4];

in vec2 tex;
uniform mat4 light_transform[4];
uniform vec3 light_dir;
uniform vec3 vpos;

layout(location = 0) out vec4 fcolor;
const float interval[4] = {0.990710020f, 0.996007917f, 0.999000371f, 1.0f};
const float bias_set[4] = {0.0048f, 0.0033f,  0.009f, 0.009f};
int t;
float calculateShadow(vec4 pos)
{
    float d = pos.w;
    int i = int(dot(ivec4(d > interval[0], d > interval[1], d > interval[2], d > interval[3]), 
    ivec4(1)));
    vec4 light_frag = light_transform[i] * vec4(pos.xyz, 1.0f);
    vec3 light_frag_pos = light_frag.xyz / light_frag.w;
    light_frag_pos = light_frag_pos * 0.5 + vec3(0.5);
    float frag_depth = light_frag_pos.z;
    
    vec2 light_tex = light_frag_pos.xy;
    vec2 shadow_map_sz = textureSize(depth[i], 0);
    float factor = 0.0f;
    vec3 frag_normal = texture(normalmap, tex).xyz;
    float bias = max((1.0f - dot(light_dir, frag_normal)) * bias_set[i] , bias_set[i] / 4.0f);

    for(int x = -3; x <= 3; x++)
        for(int y = -3; y <= 3; y++)
        {
            vec2 offset = vec2(x, y) / shadow_map_sz;
            float light_depth = texture(depth[i], light_tex + offset).r;
            factor += light_depth < frag_depth - bias ? 1.0f : 0.0f;
        }
    return factor / 49.0f;
}

vec4 gamma(vec4 color)
{
    return vec4(
        pow(color.r, 1.0f / 2.2f),
        pow(color.g, 1.0f / 2.2f),
        pow(color.b, 1.0f / 2.2f),
        color.a
    );
}

void main()
{
    vec3 normal = texture(normalmap, tex).xyz;
    vec4 rpos = texture(real_pos, tex);
    vec3 i = normalize(light_dir);
    vec3 o = normalize((vpos - rpos.xyz));
    vec3 h = normalize(i + o);
    fcolor = texture(color, tex);
    //fcolor = vec4(1.0f);
    float m = 0.2;
    float limit = 0.0000001f;
    float dhn = max(dot(h,normal), limit), din = max(dot(i, normal), limit), doh = max(dot(o, h), limit), don = max(dot(o ,normal), limit); 
    float G = min(1.0f, 2.0f * min(dhn * don /doh, dhn * din / doh));
    float F = 0.5 + 0.5 * (1 - din) * (1 - din) * (1 - din) * (1 - din) *(1 - din);
    float D = exp(- (1 - dhn * dhn) / (dhn * dhn * m * m)) / (pi * m * m * (dhn * dhn) * (dhn * dhn));

    float refelect = F * G * D / max(0.1f, 4 * din * don);

    if(dot(i, normal) < 0.0f)
        refelect = 0.0f;

    fcolor = (0.05 + vec4((0.7 / pi + 0.5   * refelect) * din)) * fcolor;
    fcolor.w = 1.0f;
    fcolor = fcolor *  (0.3 + 0.7 * (1 - calculateShadow(rpos)));
    //HDR
    fcolor = fcolor / (vec4(0.5f) + fcolor);
    // Gamma correction
    fcolor = gamma(fcolor);
} 