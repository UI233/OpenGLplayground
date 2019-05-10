#pragma once
#include "model.h"
#include "program.h"
#include "cam.h"

class Render {
public:
    static constexpr float shadowmap_sz = 512.0f;
    static constexpr int shadowmap_cnt = 4;
    void draw(Scene &scene, const Camera &cam);
    Render(GLuint width, GLuint height);
    Render() = delete;
    OpenGLProgram pre_prog, prog, shadowprog;
private:
    GLuint shadowmap[shadowmap_cnt], posmap, colormap, normalmap;
    GLuint gbuffer, shadowbuffer;
    GLuint quadvao, quadbuffer;
    mat4 light_trans[shadowmap_cnt];
    const GLuint WIDTH, HEIGHT;

    void initGBuffer();
    void initShadowBuffer();
    void initQuad();
    void renderQuad();
    void renderDeferedTexture(const Scene &scene, const Camera &cam);
    void computeShadowmap(const Scene &scene);
    void setShadowMatrix(Scene &scene, const Camera &cam);
};