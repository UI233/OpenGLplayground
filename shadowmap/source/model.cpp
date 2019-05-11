#include "model.h"
#include "stb_img.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

void Model::init(std::string path, const vector<unsigned int> &idx)
{
    if (position.empty())
        return;
    size = idx.size();
    attri_num = 1;
    glCreateVertexArrays(1, &vbo);
    glCreateBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBindVertexArray(vbo);
    
    glCreateBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * size, &idx[0], GL_STATIC_DRAW);

    std::vector<float> data;
    for (int i = 0; i < position.size(); ++i)
    {
        auto &vec = position[i];
        data.push_back(vec.x);
        data.push_back(vec.y);
        data.push_back(vec.z);
        if (i < normal.size())
        {
            auto &vec = normal[i];
            data.push_back(vec.x);
            data.push_back(vec.y);
            data.push_back(vec.z);
        }
        if (i < tex_coord.size())
        {
            auto &vec = tex_coord[i];
            data.push_back(vec.x);
            data.push_back(vec.y);
        }
    }
    glNamedBufferData(buffer, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
    int stride = 3;
    if (!normal.empty())
    {
        stride += 3;
        ++attri_num;
    }
    if (!tex_coord.empty())
    {
        stride += 2;
        ++attri_num;
    }
    int offset = 3;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)nullptr);
    if (!normal.empty())
    {
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));
        offset += 3;
    }
    if (!tex_coord.empty())
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));

    // Load texture from file
    if (path == std::string())
    {
        color_tex = 0;
        return;
    }
    int width, height, channel_num;
    auto img = stbi_load(path.c_str(), &width, &height, &channel_num, 3);
    if (!img)
        return ;
    glCreateTextures(GL_TEXTURE_2D, 1, &color_tex);
    glBindTexture(GL_TEXTURE_2D, color_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(img);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

bool Model::draw(GLuint program) const
{
    if (!glIsProgram(program))
        return false;
    glUseProgram(program);
    auto loc = glGetUniformLocation(program, "model");
    glUniformMatrix4fv(loc, 1, GL_FALSE, &model[0][0]);
    glBindVertexArray(vbo);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    for (int i = 0; i < attri_num; ++i)
        glEnableVertexAttribArray(i);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, color_tex);
    glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, nullptr);

    glUseProgram(0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return glGetError() == 0;
}


void Model::setModel(const mat4 &t_model)
{
    model = t_model;
}

std::array<float, 6> Model::getAABB() const
{
    if (position.empty())
        return std::array<float, 6>();
    float zmin, zmax, xmin, xmax, ymin, ymax;
    vec3 st = position[0];
    xmin = xmax = st.x;
    ymin = ymax = st.y;
    zmin = zmax = st.z;

    for (auto p : position)
    {
        p = model * vec4(p, 1.0f);
        xmin = std::min(xmin, p.x);
        xmax = std::max(xmax, p.x);
        ymin = std::min(ymin, p.y);
        ymax = std::max(ymax, p.y);
        zmin = std::min(zmin, p.z);
        zmax = std::max(zmax, p.z);
    }

    return std::array<float, 6>{xmin, xmax, ymin, ymax, zmin, zmax};
}

void Scene::processNode(aiNode *node, aiScene *scene)
{
    for (int i = 0; i < node->mNumMeshes; ++i)
        processMesh(scene->mMeshes[node->mMeshes[i]], scene);

    for (int i = 0; i < node->mNumChildren; ++i)
        processNode(node->mChildren[i], scene);
}

void Scene::processMesh(aiMesh *mesh, aiScene *scene)
{
    std::vector<vec3> pos, normal;
    std::vector<vec2> tex;
    for (int i = 0; i < mesh->mNumVertices; ++i)
    {
        auto &v = mesh->mVertices[i];
        pos.push_back(vec3(v.x, v.y, v.z));
        v = mesh->mNormals[i];
        normal.push_back(vec3(v.x, v.y, v.z));
        if (mesh->mTextureCoords[0])
        {
            auto &uv = mesh->mTextureCoords[0][i];
            tex.push_back(vec2(uv.x, uv.y));
        }
    }
    
    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    
    models.push_back(Model(std::move(pos), std::move(normal), std::move(tex), std::move(indices), "texture" + std::to_string(models.size()) + ".jpg"));
}

void Scene::append(std::string directory)
{
    Assimp::Importer importer;
    const auto scene = importer.ReadFile(directory, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
    processNode(scene->mRootNode, (aiScene *)scene);
}

bool Scene::draw(const GLuint program)const
{
    bool res = true;
    for (auto &model : models)
        res &= model.draw(program);
    return res;
}

void Scene::append(const Model &mesh)
{
    models.push_back(mesh);
}

void Scene::setModel(unsigned int idx, const mat4 &model)
{
    if (idx >= models.size())
        return;
    models[idx].setModel(model);
}

std::array<float, 6> Scene::getAABB() 
{
    if (models.empty())
        return std::array<float, 6>();
    if (!changed)
        return res;
    res = models[0].getAABB();
    for (unsigned int i = 1; i < models.size(); ++i)
    {
        auto temp = models[i].getAABB();
        for (unsigned int j = 0; j < 6; ++j)
        {
            if (j % 2 == 0)
                res[j] = std::min(res[j], temp[j]);
            else res[j] = std::max(res[j], temp[j]);
        }
    }

    changed = false;
    return res;
}

