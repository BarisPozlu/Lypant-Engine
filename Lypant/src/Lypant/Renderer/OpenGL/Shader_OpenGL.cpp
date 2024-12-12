#include "lypch.h"

#ifdef LYPANT_OPENGL

#include "Lypant/Renderer/Shader.h"
#include <glad/glad.h>

// TODO: this is to be updated

namespace lypant
{
    Shader::Shader(const std::string& vsPath, const std::string& fsPath)
    {
        CreateShader(vsPath, fsPath);
    }

    Shader::~Shader()
    {
        glDeleteProgram(m_Program);
    }

    void Shader::Bind() const
    {
        glUseProgram(m_Program);
    }

    int Shader::GetUniformLocation(const std::string& name) const
    {
        return glGetUniformLocation(m_Program, name.c_str());
    }

    void Shader::SetMatrix4FloatUniform(const std::string& name, float* value) const
    {
        glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, value);
    }

    void Shader::SetVec4FloatUniform(const std::string& name, float* value) const
    {
        glUniform4f(GetUniformLocation(name), value[0], value[1], value[2], value[3]);
    }

    void Shader::CreateShader(const std::string& vsPath, const std::string& fsPath)
    {
        uint32_t program = glCreateProgram();

        std::string vertexShaderSrc;
        std::string fragmentShaderSrc;
        GetShaderSource(vsPath, fsPath, vertexShaderSrc, fragmentShaderSrc);

        uint32_t vs = glCreateShader(GL_VERTEX_SHADER);
        const char* src = vertexShaderSrc.c_str();

        glShaderSource(vs, 1, &src, nullptr);
        glCompileShader(vs);

        #ifdef LYPANT_DEBUG
        GLint isCompiled = 0;
        glGetShaderiv(vs, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(vs, maxLength, &maxLength, &infoLog[0]);

            glDeleteShader(vs);

            LY_CORE_ERROR("Vertex shader could not compile.");
            LY_CORE_ASSERT(false, infoLog.data());
            return;     
        }
        #endif

        uint32_t fs = glCreateShader(GL_FRAGMENT_SHADER);
        src = fragmentShaderSrc.c_str();

        glShaderSource(fs, 1, &src, nullptr);
        glCompileShader(fs);

        #ifdef LYPANT_DEBUG
        isCompiled = 0;
        glGetShaderiv(fs, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(fs, maxLength, &maxLength, &infoLog[0]);

            glDeleteShader(vs);
            glDeleteShader(fs);

            LY_CORE_ERROR("Fragment shader could not compile.");
            LY_CORE_ASSERT(false, infoLog.data());
            return;
        }
        #endif

        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);
 
        #ifdef LYPANT_DEBUG
        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
            
            glDeleteProgram(program);
            glDeleteShader(vs);
            glDeleteShader(fs);

            LY_CORE_ASSERT(false, infoLog.data());
            return;
        }
        #endif

        glDetachShader(program, vs);
        glDetachShader(program, fs);

        m_Program = program;
    }

    void Shader::GetShaderSource(const std::string& vsPath, const std::string& fsPath, std::string& outVertexShaderSrc, std::string& outFragmentShaderSrc) const
    {
        std::ifstream stream(vsPath);
        std::string line;
        std::stringstream ss[2];

        while (std::getline(stream, line))
        {
            ss[0] << line << "\n";
        }

        stream.close();
        stream.open(fsPath);

        while (std::getline(stream, line))
        {
            ss[1] << line << "\n";
        }

        outVertexShaderSrc = ss[0].str();
        outFragmentShaderSrc = ss[1].str();
    }
}

#endif
