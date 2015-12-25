#pragma once

#include "Graphics.h"
#include "SkeletalStructs.h"
struct Vertex
{
    glm::vec3 position, normal;
    glm::vec2 texcoords;
};
struct SkinVertex
{
    glm::vec3 position, normal;
    glm::vec2 texcoords;
    SkinInfo weights;
};

class Mesh
{
public:
    Mesh();
    ~Mesh() { CleanUp(); }

    void LoadMeshData(const std::vector<Vertex> &vertices, const std::vector<unsigned short> &indices);
    void LoadMeshData(const std::vector<SkinVertex> &vertices, const std::vector<unsigned short> &indices);
    void Draw(const glm::mat4& ptransform, const glm::mat4& vtransform, const glm::mat4& mtransform);
    void CleanUp();

    void LoadBox(glm::vec3 extents, const glm::vec3 &offset = glm::vec3(0.0f));
    void LoadSphere(float radius, unsigned int rings, unsigned int sectors, const glm::vec3 &offset = glm::vec3(0.0f));

    bool HasBones() { return m_bones != NULL; }
    std::vector<Bone> &GetBones() { return *m_bones; }
    void MakeBoned() { if (!m_bones) m_bones = new std::vector<Bone>; }
private:
    bool m_loaded;

    enum Buffer_IDs { VBO, IBO, NumBuffers };
    unsigned int m_vao, m_buffers[NumBuffers], m_numIndices;
    std::vector<Bone>* m_bones;
};

