#include "lypch.h"

#ifdef LYPANT_OPENGL

#include "Lypant/Renderer/Shader.h"
#include <glad/glad.h>

namespace lypant
{
    static std::string ReadFile(const std::string& path)
    {
        std::string source;

        std::ifstream in(path, std::ios::in | std::ios::binary);
        LY_CORE_ASSERT(in, "Could not open file!");

        in.seekg(0, std::ios::end);
        source.resize(in.tellg());

        in.seekg(std::ios::beg);
        in.read(source.data(), source.size());

        in.close();
        return source;
    }

    Shader::Shader(const std::string& path)
    {
        const char* sourceArray[2];

        std::string source = ReadFile(path);

        sourceArray[0] = "#version 460 core\n#define VERTEX_SHADER\n";
        sourceArray[1] = source.c_str();

        uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 2, sourceArray, nullptr);
        glCompileShader(vertexShader);

        #ifdef LYPANT_DEBUG
        GLint isCompiled = 0;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

            glDeleteShader(vertexShader);

            LY_CORE_ERROR("Vertex shader could not compile.");
            LY_CORE_ASSERT(false, infoLog.data());
            return;
        }
        #endif
 
        sourceArray[0] = "#version 460 core\n#define FRAGMENT_SHADER\n";

        uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 2, sourceArray, nullptr);
        glCompileShader(fragmentShader);

        #ifdef LYPANT_DEBUG
        isCompiled = 0;
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

            LY_CORE_ERROR("Fragment shader could not compile.");
            LY_CORE_ASSERT(false, infoLog.data());
            return;
        }
        #endif
       
        uint32_t program = glCreateProgram();

        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);

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
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

            LY_CORE_ASSERT(false, infoLog.data());
            return;
        }
        #endif

        glDetachShader(program, vertexShader);
        glDetachShader(program, fragmentShader);

        m_Program = program;
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

    void Shader::SetUniformMatrix4Float(const std::string& name, float* value) const
    {
        glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, value);
    }

    void Shader::SetUniformMatrix3Float(const std::string& name, float* value) const
    {
        glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, value);
    }

    void Shader::SetUniformVec4Float(const std::string& name, float* value) const
    {
        glUniform4f(GetUniformLocation(name), value[0], value[1], value[2], value[3]);
    }

    void Shader::SetUniformVec3Float(const std::string& name, float* value) const
    {
        glUniform3f(GetUniformLocation(name), value[0], value[1], value[2]);
    }

    void Shader::SetUniformInt(const std::string& name, int value) const
    {
        glUniform1i(GetUniformLocation(name), value);
    }
}

#endif
