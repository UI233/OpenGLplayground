#include "glm/gtc/matrix_transform.hpp"
#include "cam.h"
#include <math.h>

Quaternion::Quaternion(vec4 value) :num(value) {}
Quaternion::Quaternion(float x, float y, float z, float w) : num(x,y,z,w) {}
vec4 Quaternion::getValue()
{
    return num;
}

float Quaternion::operator[](int idx) const
{
    if (idx >= 0 && idx <= 3)
        return num[idx];
    //Out of range
    return -1.0f;
}

Quaternion conjugation(Quaternion &a)
{
    vec4 tmp = a.getValue();
    return Quaternion(tmp[0], -tmp[1], -tmp[2], -tmp[3]);
}

Quaternion inverse(Quaternion &q)
{
    Quaternion conj = conjugation(q);
    float size = ((q*conj).getValue())[0];
    return Quaternion(conj.getValue() / sqrt(size));
}

Quaternion operator + (Quaternion a, Quaternion b)
{
    return Quaternion(a.getValue() + b.getValue());
}

Quaternion operator - (Quaternion a, Quaternion b)
{
    return Quaternion(a.getValue() - b.getValue());
}

Quaternion operator*(Quaternion a, Quaternion b)
{
    vec3 im_a(a[1], a[2], a[3]), im_b(b[1], b[2], b[3]);

    vec3 im = -cross(im_a, im_b) + a[0] * im_b + b[0] * im_a;
    float r = a[0] * b[0] - dot(im_a, im_b);

    return Quaternion(vec4(r, im));
}


Camera::Camera(float t_height, float t_width, float t_far, float t_near) : up(0.0f, 1.0f, 0.0f), front(0.0f, 0.0f, -1.0f), pos(0.0f),
per(perspective(3.1415926f / 2.0f, (float)t_width / (float)t_height, t_near, t_far)), nearv(t_near), farv(t_far), width(t_width), height(t_height){}
Camera::Camera(const vec3 &a, const vec3 &f, const vec3 &p, float t_height, float t_width, float t_far, float t_near)
    : up(a), front(f), pos(p), per(perspective(3.1415926f / 2.0f, (float)t_width / (float)t_height, t_near, t_far)), nearv(t_near), farv(t_far), width(t_width), height(t_height){}


mat4 Camera::getViewMatrix() const
{
    return per * getLookat();
}

mat4 Camera::getLookat() const
{
//    mat3 tmp(cross(front, up),up, -front);
//    tmp = transpose(tmp);
//    mat4 res(tmp);

//    res[3][3] = 1.0f;
//    res =   res * translate(mat4(1.0f), -pos);
    return lookAt(pos, pos + front, up);
}

Camera& Camera::rotation(float angel, vec3 axis)
{
    axis = normalize(axis);

    Quaternion q(vec4(cos(angel / 2.0f), sin(angel / 2.0f) * axis));

    Quaternion u(vec4(0.0f, up)), f(vec4(0.0f, front));

    Quaternion qu = (q * u)* inverse(q), qf = (q * f) * inverse(q);
    
    up = vec3(qu[1], qu[2], qu[3]);
    front = vec3(qf[1], qf[2], qf[3]);

    return *this;
}

Camera& Camera::move(vec3 offset)
{
    pos += offset;
    return *this;
}

vec3 Camera::getUp() const
{
    return up;
}
vec3 Camera::getFront() const
{
    return front;
}

vec3 Camera::getLeft() const
{
    return cross(up, front);
}


vec3 Camera::getPos() const
{
    return pos;
}

std::array<vec3, 8> Camera::getFrustum(const unsigned int &idx) const
{
    static float interval[5]{0.0f, 0.3f, 0.50f, 0.8f, 1.0f};
    std::array<vec3, 8> cube {
        vec3(1.0f, 1.0f, 1.0f),
        vec3(1.0f, 1.0f, -1.0f),
        vec3(1.0f, -1.0f, 1.0f),
        vec3(-1.0f, 1.0f, 1.0f),
        vec3(-1.0f, -1.0f, 1.0f),
        vec3(-1.0f, 1.0f, -1.0f),
        vec3(1.0f, -1.0f, -1.0f),
        vec3(-1.0f, -1.0f, -1.0f)
    };

    float far_plane = nearv + (farv - nearv) * interval[idx];
    float near_plane = nearv + (farv - nearv) * interval[idx - 1];
    mat4 inv = inverse(perspective(3.1415926f / 2.0f, (float)width / (float)height, near_plane, far_plane) * getLookat());
    vec4 d = per * vec4(0.0f, 0.0f, -far_plane, 1.0f);
    float depth = d.z / d.w;
    for (auto &d : cube)
    {
        vec4 temp = inv * vec4(d, 1.0f);
        d =  vec3(temp / temp.w);
    }

    return cube;
}