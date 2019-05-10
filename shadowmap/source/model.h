#pragma once
#include <vector>
#include "g3/include/GL/gl3w.h"
#include <assimp/scene.h>
#include <array>
#include "glm/glm.hpp"
using std::vector;
using namespace glm;

class Model {
public:
    bool draw(const GLuint program) const;
    Model():  color_tex(0), vbo(0), buffer(0), model(1.0f) {}
    Model(const vector<vec3> &t_pos, const vector<vec3> &t_normal, const vector<vec2> &t_tex, const vector<unsigned int> &idx, std::string path = std::string(), const mat4 &t_model = mat4(1.0f)) :
        position(t_pos), normal(t_normal), tex_coord(t_tex), color_tex(0), model(t_model) {
        init(path, idx);
    };
    void setModel(const mat4 &t_model);
    std::array<float, 6> getAABB() const;
private:
    void init(std::string path, const vector<unsigned int> &idx);
    vector<vec3> position;
    vector<vec3> normal;
    vector<vec2> tex_coord;
    mat4 model;
    GLuint ibo;
    GLuint color_tex;
    GLuint vbo, buffer;
    int attri_num;
    int size;
};

class Scene {
public:
    Scene():changed(true) {}
    Scene(std::string directory):changed(true) { append(directory); }
    void append(std::string directory);
    bool draw(const GLuint program) const;
    void append(const Model &mesh);
    void setModel(unsigned int idx, const mat4 &t_model);
    std::array<float, 6> getAABB();
private:
    void processNode(aiNode *node, aiScene *scene);
    void processMesh(aiMesh *mesh, aiScene *scene);
    std::vector<Model>  models;
    bool changed;
    std::array<float, 6> res;
};
