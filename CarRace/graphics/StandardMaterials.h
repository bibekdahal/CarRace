#pragma once
#include "Graphics.h"
#include "../lua/lua.h"

//// Standard Materials
class DiffuseMaterial : public MaterialBase<DIFFUSE_MATERIAL>
{
public:
    DiffuseMaterial() : m_diffuse(1.0f, 1.0f, 1.0f, 1.0f) { m_texture = Graphics::GetWhiteTexture(); }
    void SetTexture(unsigned int texture) { m_texture = texture; }
    unsigned int GetTexture() const { return m_texture; }
    void SetDiffuseColor(const glm::vec4& diffuse) { m_diffuse = diffuse; }
    glm::vec4 GetDiffuseColor() const { return m_diffuse; }

    static unsigned int AddDiffuseMaterial() { return Graphics::GetMaterialManager().Add(new DiffuseMaterial); }
    static DiffuseMaterial* GetDiffuseMaterial(unsigned int index) { return static_cast<DiffuseMaterial*>(Graphics::GetMaterialManager()[index]); }
    static void RegisterToLua(lua_State* L)
    {
        luabridge::getGlobalNamespace(L)
            .addFunction("AddDiffuseMaterial", &AddDiffuseMaterial)
            .addFunction("GetDiffuseMaterial", &GetDiffuseMaterial)
            .deriveClass<DiffuseMaterial, Material>("DiffuseMaterial")
            .addProperty("texture", &GetTexture, &SetTexture)
            .addProperty("diffuse_color", &GetDiffuseColor, &SetDiffuseColor)
            .endClass();
            ;
    }

    void Use()
    {
        GLSLProgram* program = Graphics::GetCurrentProgram();
        program->SetUniform(GLSLProgram::DIFFUSE_COLOR, m_diffuse);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Graphics::GetTextureManager()[m_texture]);
        program->SetUniform(GLSLProgram::TEXTURE_SAMPLE, 0);
    }
    void Switch()
    {
        Graphics::UseProgram(0);
        GLSLProgram* program = Graphics::GetCurrentProgram();
        unsigned int i = 0;
        for (; i < Graphics::GetLights().size(); ++i)
            program->SetLight(i, Graphics::GetLights()[i]);
        program->UnsetLights(i);
    }

private:
    glm::vec4 m_diffuse;
    unsigned int m_texture;
};
