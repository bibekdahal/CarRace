#pragma once

#include "..\utils\stdinc.h"  
#include "Light.h"   

inline std::string ReadFile(const char* filename)
{
    std::ifstream file(filename);
    if (file.fail()) 
        throw Exception(std::string("Failed to open file \"") + filename + "\"", 0);
    std::string text, temp;
    while (std::getline(file, temp))
        text += temp + "\n";
    return text;
}


class ShaderException : public Exception {
public:
    enum SHADEREXCEPTIONTYPE{ FAIL_COMPILE_SHADER, FAIL_LINK_SHADERS };
    ShaderException(std::string errorString, SHADEREXCEPTIONTYPE errorType) : Exception(errorString, errorType) {}
};

class GLSLShader
{
public:
    GLSLShader() : m_handle(0) {}
    ~GLSLShader() { DeleteShader(); }

    GLuint GetHandle() const { return m_handle; }
    void LoadShader(const char* filename, GLenum type)
    {
        std::string source = ReadFile(filename);
        const char* srcPtr = source.c_str();

        GLuint res = glCreateShader(type);
        glShaderSource(res, 1, &srcPtr, NULL);
        glCompileShader(res);

        GLint compile_ok = GL_FALSE;
        glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
        if (compile_ok == GL_FALSE) {
            char errorLog[1024] = { 0 };
            glGetShaderInfoLog(res, 1024, NULL, errorLog);
            std::string err = std::string("Failed to compile shader\n") + errorLog;
            glDeleteShader(res);
            throw ShaderException(err, ShaderException::FAIL_COMPILE_SHADER);
        }
        m_handle = res;
    }
    void DeleteShader()
    {
        if (m_handle)
        {
            glDeleteShader(m_handle);
            m_handle = 0;
        }
    }

private:
    GLuint m_handle;
};

class GLSLProgram
{
public:
    enum STANDARD3DUNIFORMINDICES { MVP_MATRIX = 0, MODEL_MATRIX, TEXTURE_SAMPLE, DIFFUSE_COLOR, 
                                    LIGHTS_START };
    enum LIGHT_PROPERTIES {
        LIGHT_IS_ENABLED = 0, LIGHT_TYPE, LIGHT_AMBIENT, LIGHT_COLOR, LIGHT_POSITION, LIGHT_CONE_DIRECTION, 
        LIGHT_SPOT_COS_CUTOFF, LIGHT_SPOT_EXPONENT, LIGHT_CONST_ATTEN, LIGHT_LINEAR_ATTEN, LIGHT_QUAD_ATTEN,
        LIGHT_PROPETIES_NUM
    };
    //enum STANDARD2DUNIFORMINDICES { UV = 1 };
    GLSLProgram() : m_handle(0) {}
    ~GLSLProgram() { DeleteProgram(); }

    GLuint GetHandle() const { return m_handle; }
    GLuint LinkShaders(const std::vector<GLSLShader> &shaders)
    {
        m_handle = glCreateProgram();
        for (unsigned i = 0; i<shaders.size(); ++i)
            glAttachShader(m_handle, shaders[i].GetHandle());
        glLinkProgram(m_handle);

        GLint success = GL_FALSE;
        glGetProgramiv(m_handle, GL_LINK_STATUS, &success);
        if (success == GL_FALSE)
        {
            char errorLog[1024] = { 0 };
            glGetProgramInfoLog(m_handle, 1024, NULL, errorLog);
            std::string err = std::string("Failed to link shaders\n") + errorLog;
            glDeleteProgram(m_handle);
            throw ShaderException(err, ShaderException::FAIL_LINK_SHADERS);
        }
        Init();
        return m_handle;
    }
    void Use() { glUseProgram(m_handle); }
    GLint GetUniformLocation(std::string uniform_name) { return glGetUniformLocation(m_handle, uniform_name.c_str()); }
 
    void SetUniform(unsigned uniform, bool boolean) { glUniform1i(m_uniforms[uniform], boolean); }
    void SetUniform(unsigned uniform, GLint integer) { glUniform1i(m_uniforms[uniform], integer); }
    void SetUniform(unsigned uniform, GLfloat number) { glUniform1f(m_uniforms[uniform], number); }
    void SetUniform(unsigned uniform, const glm::mat4 &matrix) { glUniformMatrix4fv(m_uniforms[uniform], 1, GL_FALSE, glm::value_ptr(matrix)); }
    void SetUniform(unsigned uniform, const glm::vec4 &vector) { glUniform4fv(m_uniforms[uniform], 1, glm::value_ptr(vector)); }
    void SetUniform(unsigned uniform, const glm::vec3 &vector) { glUniform3fv(m_uniforms[uniform], 1, glm::value_ptr(vector)); }

    void DeleteProgram()
    {
        if (m_handle)
        {
            glDeleteProgram(m_handle);
            m_handle = 0;
        }
    }

    struct ShaderInfo
    {
        GLenum type;
        std::string filename;
    };
    void CompileShaders(const std::vector<ShaderInfo> &shaders_info)
    {
        std::vector<GLSLShader> shaders;
        shaders.resize(shaders_info.size());
        for (unsigned i = 0; i < shaders_info.size(); ++i)
            shaders[i].LoadShader(shaders_info[i].filename.data(), shaders_info[i].type);
        LinkShaders(shaders);
    }
    struct ProgramInfo
    {
        std::vector<unsigned int> shader_ids;
        GLSLProgram * program;
    };
    static void CompileShaders(const std::vector<ShaderInfo> &shaders_info, const std::vector<ProgramInfo> &programs_info)
    {
        GLSLShader* shaders = new GLSLShader[shaders_info.size()];
        for (unsigned i = 0; i < shaders_info.size(); ++i)
            shaders[i].LoadShader(shaders_info[i].filename.data(), shaders_info[i].type);

        for (unsigned i = 0; i < programs_info.size(); ++i)
        {
            programs_info[i].program->m_handle = glCreateProgram();
            for (unsigned j = 0; j < programs_info[i].shader_ids.size(); ++j)
                glAttachShader(programs_info[i].program->m_handle, shaders[programs_info[i].shader_ids[j]].GetHandle());
            glLinkProgram(programs_info[i].program->m_handle);
            programs_info[i].program->Init();
        }
        delete[] shaders;
    }

    unsigned int AddUniform(const char* uniform_name)
    {
        m_uniforms.push_back(GetUniformLocation(uniform_name));
        return m_uniforms.size() - 1;
    }
    GLint GetUniform(unsigned int index) { return m_uniforms[index]; }
    
    void UnsetLights(unsigned int start_index = 0)
    {
        for (unsigned i = start_index; i < NUM_LIGHTS; ++i)
            SetUniform(LIGHTS_START + i*LIGHT_PROPETIES_NUM + LIGHT_IS_ENABLED, false);
    }
    void SetLight(unsigned int index, const LightProperties &light)
    {
        unsigned int i = LIGHTS_START + index*LIGHT_PROPETIES_NUM;
        SetUniform(i + LIGHT_IS_ENABLED, light.isEnabled);
        SetUniform(i + LIGHT_TYPE, light.type);
        SetUniform(i + LIGHT_AMBIENT, light.ambient);
        SetUniform(i + LIGHT_COLOR, light.color);
        SetUniform(i + LIGHT_POSITION, light.position);
        SetUniform(i + LIGHT_CONE_DIRECTION, light.coneDirection);
        SetUniform(i + LIGHT_SPOT_COS_CUTOFF, light.spotCosCutoff);
        SetUniform(i + LIGHT_SPOT_EXPONENT, light.spotExponent);
        SetUniform(i + LIGHT_CONST_ATTEN, light.constantAttenuation);
        SetUniform(i + LIGHT_LINEAR_ATTEN, light.linearAttenuation);
        SetUniform(i + LIGHT_QUAD_ATTEN, light.quadraticAttenuation);
    }

protected:
    GLuint m_handle;
    std::vector<GLint> m_uniforms;

    virtual void Init()
    {
        m_uniforms.push_back(GetUniformLocation("mvpMatrix"));
        m_uniforms.push_back(GetUniformLocation("modelMatrix"));
        m_uniforms.push_back(GetUniformLocation("texture_sample"));
        m_uniforms.push_back(GetUniformLocation("diffuse_color"));;
        for (unsigned int i = 0; i < NUM_LIGHTS; ++i)
        {
            std::string prefix = "lights[" + std::to_string(i) + "].";
            m_uniforms.push_back(GetUniformLocation(prefix + "isEnabled"));
            m_uniforms.push_back(GetUniformLocation(prefix + "type"));
            m_uniforms.push_back(GetUniformLocation(prefix + "ambient"));
            m_uniforms.push_back(GetUniformLocation(prefix + "color"));
            m_uniforms.push_back(GetUniformLocation(prefix + "position"));
            m_uniforms.push_back(GetUniformLocation(prefix + "coneDirection"));
            m_uniforms.push_back(GetUniformLocation(prefix + "spotCosCutoff"));
            m_uniforms.push_back(GetUniformLocation(prefix + "spotExponent"));
            m_uniforms.push_back(GetUniformLocation(prefix + "constantAttenuation"));
            m_uniforms.push_back(GetUniformLocation(prefix + "linearAttenuation"));
            m_uniforms.push_back(GetUniformLocation(prefix + "quadraticAttenuation"));
        }
    }
};

class Graphics;

enum MATERIAL_TYPE
{
    DIFFUSE_MATERIAL = 0,
};
class Material
{
public:
    virtual int Type() const = 0;
    virtual void Use() = 0;
    virtual void Switch() = 0;
};

template <MATERIAL_TYPE type>
class MaterialBase : public Material
{
    int Type() const { return (int)type; }
};