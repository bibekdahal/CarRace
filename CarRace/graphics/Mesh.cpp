#include "Mesh.h"

Mesh::Mesh() : m_loaded(false), m_bones(NULL)
{}

void Mesh::CleanUp()
{
    if (!m_loaded) return;
    glDeleteBuffers(NumBuffers, m_buffers);
    glDeleteVertexArrays(1, &m_vao);
    m_loaded = false;
    if (m_bones) delete m_bones;
    m_bones = NULL;
}

void Mesh::LoadMeshData(const std::vector<Vertex> &vertices, const std::vector<unsigned short> &indices)
{
    if (m_loaded) CleanUp();

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(NumBuffers, m_buffers);

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VBO]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[IBO]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texcoords));

    m_numIndices = indices.size();
    m_loaded = true;
    if (m_bones) delete m_bones;
    m_bones = NULL;
}


void Mesh::LoadMeshData(const std::vector<SkinVertex> &vertices, const std::vector<unsigned short> &indices)
{
    if (m_loaded) CleanUp();

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(NumBuffers, m_buffers);

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VBO]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SkinVertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[IBO]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SkinVertex), (GLvoid*)offsetof(SkinVertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SkinVertex), (GLvoid*)offsetof(SkinVertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SkinVertex), (GLvoid*)offsetof(SkinVertex, texcoords));

    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 4, GL_UNSIGNED_INT, sizeof(SkinVertex), (GLvoid*)(offsetof(SkinVertex, weights) + offsetof(SkinInfo, boneIds)));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(SkinVertex), (GLvoid*)(offsetof(SkinVertex, weights) + offsetof(SkinInfo, weights)));

    m_numIndices = indices.size();
    m_loaded = true;
    MakeBoned();
}

void Mesh::Draw(const glm::mat4& ptransform, const glm::mat4& vtransform, const glm::mat4& mtransform)
{
    if (!m_loaded) return;

    GLSLProgram* program = Graphics::GetCurrentProgram();
    program->SetUniform(GLSLProgram::MVP_MATRIX, ptransform*vtransform*mtransform);
    program->SetUniform(GLSLProgram::MODEL_MATRIX, mtransform);     				//transpose(inverse(mat3(model))) * vNormal;

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[IBO]);
    glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_SHORT, 0);
}

void Mesh::LoadBox(glm::vec3 extents, const glm::vec3 &offset)
{
    using namespace glm;

    float halfSizeX = extents[0];// / 2.0f;
    float halfSizeY = extents[1];// / 2.0f;
    float halfSizeZ = extents[2];// / 2.0f;
    std::vector<Vertex> vertices = {
        // front
        { vec3(-halfSizeX, -halfSizeY, halfSizeZ), vec3(0.0, 0.0, 1.0), vec2(0.0, 0.0) },
        { vec3(halfSizeX, -halfSizeY, halfSizeZ), vec3(0.0, 0.0, 1.0), vec2(1.0, 0.0) },
        { vec3(halfSizeX, halfSizeY, halfSizeZ), vec3(0.0, 0.0, 1.0), vec2(1.0, 1.0) },
        { vec3(-halfSizeX, halfSizeY, halfSizeZ), vec3(0.0, 0.0, 1.0), vec2(0.0, 1.0) },
        // top
        { vec3(-halfSizeX, halfSizeY, halfSizeZ), vec3(0.0, 1.0, 0.0), vec2(0.0, 0.0) },
        { vec3(halfSizeX, halfSizeY, halfSizeZ), vec3(0.0, 1.0, 0.0), vec2(1.0, 0.0) },
        { vec3(halfSizeX, halfSizeY, -halfSizeZ), vec3(0.0, 1.0, 0.0), vec2(1.0, 1.0) },
        { vec3(-halfSizeX, halfSizeY, -halfSizeZ), vec3(0.0, 1.0, 0.0), vec2(0.0, 1.0) },
        // back
        { vec3(halfSizeX, -halfSizeY, -halfSizeZ), vec3(0.0, 0.0, -1.0), vec2(0.0, 0.0) },
        { vec3(-halfSizeX, -halfSizeY, -halfSizeZ), vec3(0.0, 0.0, -1.0), vec2(1.0, 0.0) },
        { vec3(-halfSizeX, halfSizeY, -halfSizeZ), vec3(0.0, 0.0, -1.0), vec2(1.0, 1.0) },
        { vec3(halfSizeX, halfSizeY, -halfSizeZ), vec3(0.0, 0.0, -1.0), vec2(0.0, 1.0) },
        // bottom
        { vec3(-halfSizeX, -halfSizeY, -halfSizeZ), vec3(0.0, -1.0, 0.0), vec2(0.0, 0.0) },
        { vec3(halfSizeX, -halfSizeY, -halfSizeZ), vec3(0.0, -1.0, 0.0), vec2(1.0, 0.0) },
        { vec3(halfSizeX, -halfSizeY, halfSizeZ), vec3(0.0, -1.0, 0.0), vec2(1.0, 1.0) },
        { vec3(-halfSizeX, -halfSizeY, halfSizeZ), vec3(0.0, -1.0, 0.0), vec2(0.0, 1.0) },
        // left
        { vec3(-halfSizeX, -halfSizeY, -halfSizeZ), vec3(-1.0, 0.0, 0.0), vec2(0.0, 0.0) },
        { vec3(-halfSizeX, -halfSizeY, halfSizeZ), vec3(-1.0, 0.0, 0.0), vec2(1.0, 0.0) },
        { vec3(-halfSizeX, halfSizeY, halfSizeZ), vec3(-1.0, 0.0, 0.0), vec2(1.0, 1.0) },
        { vec3(-halfSizeX, halfSizeY, -halfSizeZ), vec3(-1.0, 0.0, 0.0), vec2(0.0, 1.0) },
        // right
        { vec3(halfSizeX, -halfSizeY, halfSizeZ), vec3(1.0, 0.0, 0.0), vec2(0.0, 0.0) },
        { vec3(halfSizeX, -halfSizeY, -halfSizeZ), vec3(1.0, 0.0, 0.0), vec2(1.0, 0.0) },
        { vec3(halfSizeX, halfSizeY, -halfSizeZ), vec3(1.0, 0.0, 0.0), vec2(1.0, 1.0) },
        { vec3(halfSizeX, halfSizeY, halfSizeZ), vec3(1.0, 0.0, 0.0), vec2(0.0, 1.0) },
    };

    if (offset != glm::vec3(0.0f))
    for (unsigned i = 0; i < vertices.size(); ++i)
        vertices[i].position += offset;

    std::vector<unsigned short> indices = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        8, 9, 10, 8, 10, 11,
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23,
    };
    LoadMeshData(vertices, indices);
}

void Mesh::LoadSphere(float radius, unsigned int rings, unsigned int sectors, const glm::vec3 &offset)
{
    const float R = 1.0f / (float)(rings - 1);
    const float S = 1.0f / (float)(sectors - 1);
    unsigned int r, s;

    std::vector<Vertex> vertices;
    vertices.resize(rings*sectors);

    int i = 0;
    for (r = 0; r < rings; ++r)
    for (s = 0; s < sectors; ++s) {
#define M_PI (3.14159265359f)
#define M_PI_2 (1.57079632679f)
        float const y = -sin(-M_PI_2 + M_PI * r * R);
        float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
        float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

        vertices[i].texcoords.x = s*S;
        vertices[i].texcoords.y = r*R;

        vertices[i].position.x = x * radius;
        vertices[i].position.y = y * radius;
        vertices[i].position.z = z * radius;

        vertices[i].normal.x = x;
        vertices[i].normal.y = y;
        vertices[i].normal.z = z;
        ++i;
    }

    if (offset != glm::vec3(0.0f))
    for (unsigned i = 0; i < vertices.size(); ++i)
        vertices[i].position += offset;

    std::vector<unsigned short> indices;
    indices.resize(rings * sectors * 6);
    std::vector<unsigned short>::iterator id = indices.begin();

    for (r = 0; r < rings; ++r)
    for (s = 0; s < sectors; ++s) {
        *id++ = r * sectors + s;
        *id++ = r * sectors + (s + 1);
        *id++ = (r + 1) * sectors + (s + 1);
        *id++ = r * sectors + s;
        *id++ = (r + 1) * sectors + (s + 1);
        *id++ = (r + 1) * sectors + s;
    }

    LoadMeshData(vertices, indices);
}