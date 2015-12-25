#pragma once


#include "Mesh.h"
#include "btBulletDynamicsCommon.h"    // Remove this later


class Model
{
public:
    struct MeshInfo
    {
        MeshInfo() {}
        MeshInfo(unsigned int mesh, unsigned int material = 0, const glm::mat4& offset = glm::mat4(1.0f)) 
            : mesh(mesh), material(material), offset(offset) {}
        unsigned int mesh;
        unsigned int material;
        glm::mat4 offset;
    };

    Model() : m_rootNode(NULL)  {}

    unsigned int AddMeshInfo(const MeshInfo &meshInfo) { m_meshes.push_back(meshInfo); return m_meshes.size() - 1; }
    unsigned int AddMesh(unsigned int mesh, unsigned int material = 0, const glm::mat4& offset = glm::mat4(1.0f)) 
    { m_meshes.push_back(MeshInfo(mesh, material, offset)); return m_meshes.size() - 1; }
    MeshInfo& GetMeshInfo(unsigned int index) { return m_meshes[index]; }
    unsigned int GetMeshesSize() const { return m_meshes.size(); }

    unsigned int GetMesh(unsigned int index) const { return m_meshes[index].mesh; }
    unsigned int GetMaterial(unsigned int index) const { return m_meshes[index].material; }
    const glm::mat4& GetOffset(unsigned int index) const { return m_meshes[index].offset; }
    void SetMesh(unsigned int index, unsigned int mesh) { m_meshes[index].mesh = mesh; }
    void SetMaterial(unsigned int index, unsigned int material) { m_meshes[index].material = material; }
    void SetOffset(unsigned int index, const glm::mat4& offset) { m_meshes[index].offset = offset; }

    void LoadFromFile(std::fstream &file, glm::vec3* min = NULL, glm::vec3* max = NULL, bool autocenter = false, const glm::mat4& offset=glm::mat4(1.0f));
    void LoadFromFile(std::string filename, glm::vec3* min = NULL, glm::vec3* max = NULL, bool autocenter = false, const glm::mat4& offset = glm::mat4(1.0f));

    AnimationNode* GetRootNode() const { return m_rootNode; }
    unsigned int GetNumNodes() const { return m_numNodes; }
    // For script:
    static unsigned int AddModel(std::string filename) 
    { 
        Model* model = new Model();
        model->LoadFromFile(filename);
        return Graphics::GetModelManager().Add(model); 
    }
    static unsigned int AddModelEx(std::string filename, glm::vec3* min, glm::vec3* max, bool autocenter, const glm::mat4& offset)
    {
        Model* model = new Model();
        model->LoadFromFile(filename, min, max, autocenter, offset);
        return Graphics::GetModelManager().Add(model);
    }
    static unsigned int Duplicate(unsigned int original)
    {
        Model* model = Graphics::GetModelManager()[original];
        Model* nmodel = new Model;
        nmodel->m_meshes = model->m_meshes;
        return Graphics::GetModelManager().Add(nmodel);
    }
    static Model* GetModel(unsigned int index) { return Graphics::GetModelManager()[index]; }
    static Mesh* GetMeshPtr(unsigned int index) { return Graphics::GetMeshManager()[index]; }

    static unsigned int AddBoxMesh(const glm::vec3& extents)
    {
        Mesh* mesh = new Mesh();
        mesh->LoadBox(extents);
        return Graphics::GetMeshManager().Add(mesh);
    }
    static unsigned int AddSphereMesh(float radius, unsigned int rings, unsigned int sectors)
    {
        Mesh* mesh = new Mesh();
        mesh->LoadSphere(radius, rings, sectors);
        return Graphics::GetMeshManager().Add(mesh);
    }
protected:
    std::vector<MeshInfo> m_meshes;
    AnimationNode* m_rootNode;
    unsigned int m_numNodes;
    //std::vector<unsigned int> m_materials;
};