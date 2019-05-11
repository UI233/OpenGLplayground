#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <array>
using namespace glm;

class Quaternion
{
public:
    Quaternion(vec4 quan);
    Quaternion(float x, float y, float z, float w);
    Quaternion() = default;
    ~Quaternion() = default;
    float operator[](int idx) const;
    vec4 getValue();
private:
    vec4 num;
};

Quaternion inverse(Quaternion &);
Quaternion conjugation(Quaternion &);
Quaternion operator - (Quaternion a, Quaternion b);
Quaternion operator + (Quaternion a, Quaternion b);
Quaternion operator * (Quaternion a, Quaternion b);


class Camera
{
public:
    Camera(float height = 720, float width = 1280, float far = 50.0f, float near = 0.1f);
    Camera(const vec3 &a, const vec3 &f, const vec3 &p, float height = 720, float width = 1280, float far = 50.0f, float near = 0.1f);
    ~Camera() = default;
    mat4 getViewMatrix() const;//get the world to view transformation matrix
    mat4 getLookat() const;
    Camera& rotation(float angel, vec3 axis); //rotate the camera aroud given axis
    Camera& move(vec3 offset);//translate the camera
    vec3 getUp() const;
    vec3 getFront() const;
    vec3 getLeft() const;
    vec3 getPos() const;
    std::array<vec3, 8> getFrustum(const unsigned int &idx) const;
    
private:
    float width;
    float height;
    float nearv;
    float farv;
    mat4 per;
    vec3 up;//the direction over 'head'
    vec3 front;//the direction it looks at
    vec3 pos;//the position of the camera
};