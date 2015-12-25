#pragma once
#include "../utils/stdinc.h"
#include "../utils/SOIL.h"

class TextureException : public Exception {
public:
    enum TEXTUREEXCEPTIONTYPE{ FAIL_SOIL_TEXTURE_LOAD };
    TextureException(std::string errorString, int errorType) : Exception(errorString, errorType) {}
};

class TextureManager
{
public:
    unsigned int AddTexture(GLuint texture) { m_textures.push_back(texture); return m_textures.size() - 1; }
    unsigned int AddTexture(std::string filename)
    {
        unsigned int texture = SOIL_load_OGL_texture(filename.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
        if (texture == 0)
            throw TextureException("ERROR loading texture: " + filename + "\n" + SOIL_last_result(), TextureException::FAIL_SOIL_TEXTURE_LOAD);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        m_textures.push_back(texture);
        return m_textures.size()-1;
    }
    GLuint GetTexture(unsigned int index) { return m_textures[index]; }
    GLuint operator[] (unsigned int index) { return GetTexture(index); }

    void CleanUp() 
    {
        if (m_textures.size() > 0) 
            glDeleteTextures(m_textures.size(), &m_textures[0]);
        m_textures.clear();
    }

private:
    std::vector<unsigned int> m_textures;
};

