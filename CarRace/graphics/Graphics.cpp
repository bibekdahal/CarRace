#include "Graphics.h"

std::vector<glm::mat4> Graphics::m_matrices;
unsigned int Graphics::m_whiteTexture;
TextureManager Graphics::m_textureManager;
Manager<GLSLProgram> Graphics::m_programManager;
Manager<Material> Graphics::m_materialManager;
Manager<Mesh> Graphics::m_meshManager;
Manager<Model> Graphics::m_modelManager;
unsigned int Graphics::m_currentProgram = -1;
RenderLayer Graphics::m_currentLayer = DefaultLayer; 
std::vector<LightProperties> Graphics::m_lights;

void Graphics::Initialize()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    static const float pixels[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLuint wt;
    glGenTextures(1, &wt);
    glBindTexture(GL_TEXTURE_2D, wt);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, 1, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGBA, GL_FLOAT, pixels);
    m_whiteTexture= m_textureManager.AddTexture(wt);

    glClearColor(0.396f, 0.612f, 0.937f, 1.0f);
}

void Graphics::CleanUp()
{
}

void Graphics::CleanUpManagers()
{
    m_textureManager.CleanUp();
    m_programManager.CleanUp();
    m_materialManager.CleanUp();
    m_matrices.clear();
}
