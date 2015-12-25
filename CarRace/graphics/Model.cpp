#include "Model.h"
#include "../graphics/StandardMaterials.h"

static void ReadNode(std::fstream &file, AnimationNode* node, AnimationNode* parent = NULL)
{
    file.read((char*)&node->id, sizeof(node->id));
    unsigned int sz;
    file.read((char*)&sz, sizeof(sz));
    node->meshIds.resize(sz);
    for (unsigned int i = 0; i < sz; ++i)
        file.read((char*)&node->meshIds[i], sizeof(unsigned int));
    file.read((char*)&sz, sizeof(sz));
    node->children.resize(sz);
    node->parent = parent;
    for (unsigned int i = 0; i < sz; ++i)
        ReadNode(file, &node->children[i], node);
}
void Model::LoadFromFile(std::fstream &file, glm::vec3* imin, glm::vec3* imax, bool autocenter, const glm::mat4& offset)
{
    struct Header
    {
        int magic;
        int version;
        int type;
    } header;

    file.read((char*)&header, sizeof(Header));
    if (header.magic != 0xb1b3cda1)
        throw Exception("Bad Model File", 0);

    m_rootNode = new AnimationNode;
    if (header.type == 2)
    {
        ReadNode(file, m_rootNode);
        file.read((char*)&m_numNodes, sizeof(m_numNodes));
    }

    glm::vec3 min, max;
    unsigned int numMaterials;
    file.read((char*)&numMaterials, sizeof(numMaterials));

    std::vector<unsigned int> mats;
    for (unsigned i = 0; i < numMaterials; ++i)
    {
        unsigned int len;
        file.read((char*)&len, sizeof(len));
        char* cstr = new char[len + 1];
        file.read(cstr, len);
        cstr[len] = '\0';
        std::string str(cstr);
        delete[] cstr;

        struct Color { float r, g, b; } color;
        file.read((char*)&color, sizeof(Color));
        if (str != "$NULL$" && str != "")
        {
            DiffuseMaterial * newmat = new DiffuseMaterial;
            newmat->SetDiffuseColor(glm::vec4(color.r, color.g, color.b, 1));
            newmat->SetTexture(Graphics::GetTextureManager().AddTexture("textures/" + str));
            mats.push_back(Graphics::GetMaterialManager().Add(newmat));
        }
        else if (color.r == 1.0f && color.g == 1.0f && color.b == 1.0f)
            mats.push_back(0);
        else
        {
            DiffuseMaterial * newmat = new DiffuseMaterial;
            newmat->SetDiffuseColor(glm::vec4(color.r, color.g, color.b, 1));
            newmat->SetTexture(Graphics::GetWhiteTexture());
            mats.push_back(Graphics::GetMaterialManager().Add(newmat));
        }
    }
    std::vector<Vertex> vertices;
    std::vector<SkinVertex> skinvertices;
    std::vector<unsigned short> indices;

    unsigned int numMeshes;
    file.read((char*)&numMeshes, sizeof(numMeshes));
    for (unsigned i = 0; i < numMeshes; ++i)
    {
        unsigned int numBones = 0;
        if (header.type == 2)
            file.read((char*)&numBones, sizeof(numBones));

        Mesh * mesh = new Mesh;
        unsigned int matId;
        file.read((char*)&matId, sizeof(matId));
        AddMesh(Graphics::GetMeshManager().Add(mesh), mats[matId]);

        unsigned int numVertices;
        file.read((char*)&numVertices, sizeof(numVertices));
        if (numBones > 0) skinvertices.resize(numVertices);
        else vertices.resize(numVertices);

        glm::vec3* positions = new glm::vec3[numVertices];
        file.read((char*)positions, sizeof(glm::vec3)* numVertices);
        glm::vec3* normals = new glm::vec3[numVertices];
        file.read((char*)normals, sizeof(glm::vec3)* numVertices);
        glm::vec2* tcoords = new glm::vec2[numVertices];
        file.read((char*)tcoords, sizeof(glm::vec2)*numVertices);

        for (unsigned j = 0; j < numVertices; ++j)
        {
            if (numBones>0)
            {
                skinvertices[j].position = positions[j];
                skinvertices[j].normal = normals[j];
                skinvertices[j].texcoords = tcoords[j];
            }
            else
            {
                vertices[j].position = positions[j];
                vertices[j].normal = normals[j];
                vertices[j].texcoords = tcoords[j];
            }

            if (imin || autocenter) {
                if (i == 0 && j == 0) min = vertices[j].position;
                else {
                    if (vertices[j].position.x < min.x) min.x = vertices[j].position.x;
                    if (vertices[j].position.y < min.y) min.y = vertices[j].position.y;
                    if (vertices[j].position.z < min.z) min.z = vertices[j].position.z;
                }
            }
            if (imax || autocenter) {
                if (i == 0 && j == 0) max = vertices[j].position;
                else {
                    if (vertices[j].position.x > max.x) max.x = vertices[j].position.x;
                    if (vertices[j].position.y > max.y) max.y = vertices[j].position.y;
                    if (vertices[j].position.z > max.z) max.z = vertices[j].position.z;
                }
            }
        }
        delete[] positions; delete[] normals; delete[] tcoords;

        unsigned int numIndices;
        file.read((char*)&numIndices, sizeof(numIndices));

        unsigned int* meshIndices = new unsigned int[numIndices];
        file.read((char*)meshIndices, sizeof(unsigned int)*numIndices);
        indices.resize(numIndices);
        for (unsigned j = 0; j < numIndices; ++j)
            indices[j] = (unsigned short)meshIndices[j];

        delete[] meshIndices;

        if (numBones>0)
        {
            mesh->MakeBoned();
            std::vector<Bone> &bones = mesh->GetBones();
            for (unsigned int b = 0; b < numBones; ++b)
            {
                Bone* bone = &bones[b];
                file.read((char*)&bone->nodeId, sizeof(bone->nodeId));
                file.read((char*)&bone->offset[0][0], 4 * 4 * sizeof(float));
            }

            std::vector<SkinInfo> skinInfos;
            skinInfos.resize(numVertices);
            file.read((char*)&skinInfos[0], sizeof(SkinInfo)*numVertices);
            for (unsigned int v = 0; v < numVertices; ++v)
                skinvertices[v].weights = skinInfos[v];
            mesh->LoadMeshData(skinvertices, indices);
        }
        else
            mesh->LoadMeshData(vertices, indices);
    }

    if (autocenter)
    {
        glm::vec3 center = (min + max) / 2.0f;
        glm::vec3 extents = (max - min) / 2.0f;
        for (unsigned i = 0; i < GetMeshesSize(); ++i)
            m_meshes[i].offset =  glm::translate(glm::mat4(), -center);
    }

    for (unsigned i = 0; i < GetMeshesSize(); ++i)
        m_meshes[i].offset = offset * m_meshes[i].offset;

    if (imin) *imin = min;
    if (imax) *imax = max;
}


void Model::LoadFromFile(std::string filename, glm::vec3* min, glm::vec3* max, bool autocenter, const glm::mat4& offset)
{
    std::fstream file;
    file.open(filename, std::ios::in | std::ios::binary);
    if (file.good())
        LoadFromFile(file, min, max, autocenter, offset);
    else
        throw Exception("Could not open model file", 0);
    file.close();
}