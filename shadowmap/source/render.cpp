#include "render.h"
using namespace glm;

Render::Render(GLuint width, GLuint height):WIDTH(width), HEIGHT(height) {
    initGBuffer();
    initQuad();
    initShadowBuffer();
}

void Render::initQuad()
{
    float quads[6][3] = {
       {-1.0f, 1.0f, 0.0f},
       {-1.0f, -1.0f, 0.0f},
       {1.0f, 1.0f, 0.0f},
       {-1.0f, -1.0f, 0.0f},
       {1.0f, -1.0f, 0.0f},
       {1.0f, 1.0f, 0.0f}
    };
    glCreateVertexArrays(1, &quadvao);
    glCreateBuffers(1, &quadbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quadvao);
    glBindVertexArray(quadbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quads), quads, GL_STATIC_DRAW);
    //The coordinate of vertices' position
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void Render::initShadowBuffer()
{
    glCreateFramebuffers(1, &shadowbuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowbuffer);
    for (int i = 0; i < shadowmap_cnt; ++i) 
    {
        glCreateTextures(GL_TEXTURE_2D, 1, shadowmap + i);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, shadowmap[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, (shadowmap_cnt - i) * shadowmap_sz, (shadowmap_cnt - i) * shadowmap_sz, 0, GL_DEPTH_COMPONENT,
            GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        auto err = glGetError();
        err = err;
    }
       
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    auto err = glGetError();
}

void Render::initGBuffer()
{
    static GLenum frame_buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glCreateFramebuffers(1, &gbuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gbuffer);
    static auto init2Dtex = [](unsigned int idx, GLuint tex) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + idx, GL_TEXTURE_2D, tex, 0);
    };
    // The position GBuffer
    glCreateTextures(GL_TEXTURE_2D, 1, &posmap);
    glBindTexture(GL_TEXTURE_2D, posmap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
    init2Dtex(0, posmap);
    // The color GBuffer 
    glCreateTextures(GL_TEXTURE_2D, 1, &colormap);
    glBindTexture(GL_TEXTURE_2D, colormap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
    init2Dtex(1, colormap);
    // The normal GBuffer
    glCreateTextures(GL_TEXTURE_2D, 1, &normalmap);
    glBindTexture(GL_TEXTURE_2D, normalmap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, nullptr);
    init2Dtex(2, normalmap);
    
    glDrawBuffers(3, frame_buffers);
    // bind Deapth Buffer
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
}


void Render::renderQuad()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0,  WIDTH, HEIGHT);
    glUseProgram(prog);

    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0, 1.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(prog);
    glBindVertexArray(quadvao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, posmap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, colormap);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, normalmap);
    for (int i = 0; i < shadowmap_cnt; ++i)
    {
        glActiveTexture(GL_TEXTURE3 + i);
        glBindTexture(GL_TEXTURE_2D, shadowmap[i]);
        prog.setUniform("depth[" + std::to_string(i) + "]", 3 + i);
        prog.setUniform("light_transform[" + std::to_string(i) + "]", light_trans[i]);
    }

    //mat4 temp;
    //auto loc = glGetUniformLocation(prog, "light_transform");
    //glGetUniformfv(prog, loc, value_ptr(temp));
    vec3 light_dir(0.0f, -1.0f, 1.0f);
    light_dir = normalize(light_dir);
    prog.setUniform("light_dir", light_dir);
    prog.setUniform("real_pos", 0);
    prog.setUniform("color", 1);
    prog.setUniform("normalmap", 2);

    glBindBuffer(GL_ARRAY_BUFFER, quadbuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    auto err = glGetError();
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
}

void Render::computeShadowmap(const Scene &scene)
{
    glUseProgram(shadowprog);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowbuffer);
    for (int i = 0; i < shadowmap_cnt; ++i)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, shadowmap[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowmap[i], 0);
        shadowprog.setUniform("transform", light_trans[i]);
        glClear(GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, (shadowmap_cnt - i) * shadowmap_sz, (shadowmap_cnt - i) * shadowmap_sz);
        scene.draw(shadowprog);
    }
    glCullFace(GL_BACK);
}

void Render::renderDeferedTexture(const Scene &scene, const Camera &cam)
{
    mat4 transform =  cam.getViewMatrix();
    glUseProgram(pre_prog);
    glBindFramebuffer(GL_FRAMEBUFFER, gbuffer);
    glViewport(0, 0,  WIDTH, HEIGHT);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    pre_prog.setUniform("transform", transform);
    scene.draw(pre_prog);
}

void Render::draw(Scene &scene, const Camera &cam)
{
    if (!prog.isValid() || !pre_prog.isValid() || !shadowprog.isValid())
        throw "Invalid program";
    setShadowMatrix(scene, cam);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    computeShadowmap(scene);
    renderDeferedTexture(scene, cam);
    // draw a fullscreen quad to render the scene to Screen
    prog.setUniform("vpos", cam.getPos());
    renderQuad();
    glBindVertexArray(0);
}

void Render::setShadowMatrix(Scene &scene, const Camera &cam)
{
    mat4 lookat = lookAt(vec3(0.0f, 110.0f, 110.0f), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
    auto coord = scene.getAABB();
    std::array<vec3, 8> AABB{
        vec3(coord[0], coord[2], coord[4]),
        vec3(coord[0], coord[2], coord[5]),
        vec3(coord[0], coord[3], coord[4]),
        vec3(coord[1], coord[2], coord[4]),
        vec3(coord[1], coord[3], coord[4]),
        vec3(coord[1], coord[2], coord[5]),
        vec3(coord[0], coord[3], coord[5]),
        vec3(coord[1], coord[3], coord[5])
    };
    float zmin_aabb(INFINITY), zmax_aabb(-INFINITY);
    for (auto p : AABB)
    {
        vec4 temp  = lookat * vec4(p, 1.0f);
        p = vec3(temp) / temp.w;
        zmin_aabb = std::min(zmin_aabb, p.z);
        zmax_aabb = std::max(zmax_aabb, p.z);
    }

    for (int i = 1; i <= shadowmap_cnt; ++i)
    {
        float xmin(INFINITY), xmax(-INFINITY), ymin(INFINITY), ymax(-INFINITY), zmin(zmin_aabb), zmax(zmax_aabb);
        auto frustum = cam.getFrustum(i);
        for (auto p : frustum)
        {
            vec4 temp  = lookat * vec4(p, 1.0f);
            p = vec3(temp) / temp.w;
            xmin = std::min(xmin, p.x);
            xmax = std::max(xmax, p.x);
            ymin = std::min(ymin, p.y);
            ymax = std::max(ymax, p.y);
            zmin = std::min(zmin, p.z);
            zmax = std::max(zmax, p.z);
        }

        light_trans[i - 1] = ortho(xmin, xmax, ymin, ymax, -zmax , -zmin) * lookat;
    }
    // light_trans = ortho(-10.0f, 10.0f, -10.0f, 10.0f, 5.1f - 0.005f, 20.0f + 0.005f) * lookat;
    //shadowprog.setUniform("transform", light_trans);
    //prog.setUniform("light_transform", light_trans);
}
