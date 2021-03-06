#include "pch.h"
#include <iostream>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_img.h"
#include "g3/include/GL/gl3w.h"
#include "include/GLFW/glfw3.h"
#include "g3/include/GL/glcorearb.h"
#include "cam.h"
#include "model.h"
#include <functional>
#include "render.h"
#define PI 3.1415926f
#define WIDTH 1280
#define HEIGHT 720
using std::vector;
using namespace glm;
Camera cam;
vec3 light_dir = vec3(0.0f, 0.0f, -1.0f);

//Initialize the rendering program : load, complie shaders and link program.
GLuint initProgram(Render &render);
void processInput(Camera &cam, GLFWwindow *window, float deltaTime);
void processMouse(GLFWwindow* window, double xpos, double ypos);
GLFWwindow* configureEnvironment();

int main()
{
    auto window = configureEnvironment();
    Scene scene;
    Render render(WIDTH, HEIGHT);
    scene.append("thorhammer.obj");
    //scene.append("cube.obj");
    initProgram(render);
    int count = 0;
    cam.move(vec3(0.0f, 0.0f, 5.0f));
    float last_time(glfwGetTime());
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    while (!glfwWindowShouldClose(window))
    {
        auto now = glfwGetTime();
        render.draw(scene, cam);
        processInput(cam, window, now - last_time);
        glfwSwapBuffers(window);
        glfwPollEvents();
        last_time = now;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}

GLuint initProgram(Render &render)
{
    render.prog = OpenGLProgram(true, "shaders/sun.vert", GL_VERTEX_SHADER,"shaders/sun.frag", GL_FRAGMENT_SHADER);
    render.pre_prog = OpenGLProgram(true, "shaders/pre.vert", GL_VERTEX_SHADER, "shaders/pre.frag", GL_FRAGMENT_SHADER);
    render.shadowprog = OpenGLProgram(true,"shaders/shadow.vert", GL_VERTEX_SHADER, "shaders/shadow.frag", GL_FRAGMENT_SHADER);
    return 0;
}

/*
void initSphere(vector<Sphere *> &tmp)
{
    Sphere *star;
    
    star = new Sun(5.3, vec3(0.0, 0.0, -23.0f), vec3(0.0, 0.0, 0.0)
        , vec3(0.0, 0.0, 0.0));

    tmp.push_back(star);
    //Earth
    star = new Planet(1.15f, 0.6f, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 48.8f, 0.0f)
        , vec3(0.0f, 0.0f, 32.8f), *tmp[0]);

    tmp.push_back(star);
    //Another planet
    star = new Planet(2.15f, 0.25f, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 24.0f, -1.5f)
        , vec3(52.0f, 0.0f, 0.0f), *tmp[0]);

    tmp.push_back(star);
    //the moon
    star = new Satellite(0.35f, 3.8f,vec3(0.0f, 0.0f, 0.0f), vec3(2.5f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 1.5f), *tmp[1]);

    tmp.push_back(star);
}
*/

void processInput(Camera &cam,  GLFWwindow *window, float deltaTime)
{
    deltaTime *= 10.0f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cam.move(cam.getFront() * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam.move(-cam.getFront() * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam.move(cam.getLeft() * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam.move(-cam.getLeft() * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cam.rotation(-deltaTime * 0.1f, cam.getFront());
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cam.rotation(deltaTime * 0.1f, cam.getFront());

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void processMouse(GLFWwindow* window, double xpos, double ypos)
{
    static const float delta_theta(PI / 400.0f);
    static bool first_mouse(true);
    static double last_x(0.0), last_y(0.0);
    if (first_mouse)
    {
        first_mouse = false;
        last_x = xpos;
        last_y = ypos;
    }

    float x_off = xpos - last_x;
    float y_off = ypos - last_y;

    last_x = xpos;
    last_y = ypos;

    cam.rotation(delta_theta * x_off, cam.getUp());
    cam.rotation(-delta_theta * y_off, cam.getLeft());
}


GLFWwindow* configureEnvironment()
{
    glfwInit();
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "minimal", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, processMouse);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    gl3wInit();
    return window;
}