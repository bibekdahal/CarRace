#pragma once
#include "GLSLShader.h"
#include "TextureManager.h"
#include "Layers.h"

class Mesh;
class Model;
class Graphics
{
public:
    ~Graphics() { CleanUp(); }
    void Initialize();
    void CleanUp();
    void Resize(float width, float height) { m_width = width; m_height = height; }
    void ChangeViewport(int x, int y, int width, int height) { glViewport(x, y, width, height); }

    void BeginRender() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); m_lights.clear(); }

    float GetWindowWidth() const { return m_width; }
    float GetWindowHeight() const { return m_height; }

    void FirstPass() { m_passOwner = m_passId = m_pass = 0; }
    bool IsValidPass() { return m_pass < m_totalPasses; }
    void NextPass() 
    { 
        m_pass++; m_passId++;
        if (m_passId == m_passes[m_passOwner])
        {
            m_passId = 0;
            m_passOwner++;
        }
    }
    unsigned int AddPassOwner() { m_passes.push_back(0); return m_passes.size() - 1; }
    unsigned int GetTotalPasses(unsigned int owner) const { return m_passes[owner]; }
    void SetTotalPasses(unsigned int owner, unsigned int passes) { m_passes[owner] = passes; RecalculatePasses(); }
    unsigned int GetTotalPasses() { return m_totalPasses; }
    unsigned int GetCurrentPass() { return m_pass; }
    unsigned int GetCurrentPassOwner() { return m_passOwner; }

    static std::vector<LightProperties>& GetLights() { return m_lights; }

    static unsigned int GetWhiteTexture() { return m_whiteTexture; }
    static TextureManager& GetTextureManager() { return m_textureManager; }
    static Manager<Material>& GetMaterialManager() { return m_materialManager; }
    static Manager<GLSLProgram>& GetProgramManager() { return m_programManager; }
    static Manager<Mesh>& GetMeshManager() { return m_meshManager; }
    static Manager<Model>& GetModelManager() { return m_modelManager; }

    static void UseProgram(unsigned int index) { m_programManager[index]->Use(); m_currentProgram = index; }
    static GLSLProgram* GetCurrentProgram() { return m_programManager[m_currentProgram]; }
    static unsigned int GetCurrentProgramIndex() { return m_currentProgram; }

    static unsigned int AddMatrix(const glm::mat4 &matrix = glm::mat4()) { m_matrices.push_back(matrix); return m_matrices.size() - 1; }
    static glm::mat4& GetMatrix(unsigned int index) { return m_matrices[index]; }

    static RenderLayer GetCurrentLayer() { return m_currentLayer; }
    static void SetCurrentLayer(RenderLayer layer) { m_currentLayer = layer; }

    static void CleanUpManagers();



    static Material* GetMaterial(unsigned int index) { return m_materialManager[index]; }
private:
    static std::vector<glm::mat4> m_matrices;
    static unsigned int m_whiteTexture;
    static TextureManager m_textureManager;
    static Manager<GLSLProgram> m_programManager;
    static Manager<Material> m_materialManager;
    static Manager<Mesh> m_meshManager;
    static Manager<Model> m_modelManager;
    static unsigned int m_currentProgram;
    static RenderLayer m_currentLayer;
    static std::vector<LightProperties> m_lights;

    float m_width, m_height;
    std::vector<unsigned int> m_passes;
    unsigned int m_totalPasses;
    unsigned int m_passId, m_passOwner, m_pass;

    void RecalculatePasses()
    {
        m_totalPasses = 0;
        for (unsigned int i = 0; i < m_passes.size(); ++i)
            m_totalPasses += m_passes[i];
    }
};

