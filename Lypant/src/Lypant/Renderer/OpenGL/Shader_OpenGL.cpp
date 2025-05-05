#include "lypch.h"

#ifdef LYPANT_OPENGL

#include "Lypant/Renderer/Shader.h"
#include <glad/glad.h>

namespace lypant
{
    static ShaderDataType GetTypeFromOpenGLType(GLenum type)
    {
        switch (type)
        {
            case GL_FLOAT:          return ShaderDataType::Float;
            case GL_FLOAT_VEC2:     return ShaderDataType::Float2;
            case GL_FLOAT_VEC3:     return ShaderDataType::Float3;
            case GL_FLOAT_VEC4:     return ShaderDataType::Float4;
            case GL_FLOAT_MAT3:     return ShaderDataType::Mat3;
            case GL_FLOAT_MAT4:     return ShaderDataType::Mat4;
            case GL_INT:            return ShaderDataType::Int;
            case GL_BOOL:           return ShaderDataType::Bool;
            case GL_SAMPLER_2D:     return ShaderDataType::Sampler2D;
            case GL_SAMPLER_CUBE:   return ShaderDataType::Samplercube;
        }

        LY_CORE_ASSERT(false, "Invalid OpenGL data type!");
        return ShaderDataType::None;
    }

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

    Shader::Shader(const std::string& path) : m_Path(path)
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

        PopulateUniformTypes();

        Bind();

        int samplerValue = 0;

        for (auto& [name, type] : m_UniformNamesToTypesMap)
        {
            if (type == ShaderDataType::Samplercube)
            {
                if (name.find("DiffuseIrradiance") != std::string::npos)
                {
                    SetUniformInt(name, 0);
                    samplerValue++;
                }

                else if (name.find("PreFilter") != std::string::npos)
                {
                    SetUniformInt(name, 1);
                    samplerValue++;
                }
            }

            else if (type == ShaderDataType::Sampler2D && name.find("BRDF") != std::string::npos)
            {
                SetUniformInt(name, 2);
                samplerValue++;
            }
        }

        for (auto& [name, type] : m_UniformNamesToTypesMap)
        {
            if (type == ShaderDataType::Sampler2D && name.find("BRDF") == std::string::npos)
            {
                SetUniformInt(name, samplerValue++);
            }
        }
    }

    Shader::~Shader()
    {
        glDeleteProgram(m_Program);
        s_Cache.erase(m_Path);
    }

    void Shader::PopulateUniformTypes()
    {
        int activeUniforms;
        glGetProgramiv(m_Program, GL_ACTIVE_UNIFORMS, &activeUniforms);

        int maxLength;
        glGetProgramiv(m_Program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);

        char* nameBuffer = new char[maxLength];

        GLenum currentType;

        int isUniformBlock;
        int length;
        int size; // Does not work for arrays right now beacuse of this.
        // might wanna actually add to the name at the end like this: [size] this will enable the shaders to be written freely but we will be able to see if they are
        // an array and their size without any more additionial overhead.

        for (unsigned int i = 0; i < activeUniforms; i++)
        {
            glGetActiveUniformsiv(m_Program, 1, &i, GL_UNIFORM_BLOCK_INDEX, &isUniformBlock);
            if (isUniformBlock == -1)
            {
                glGetActiveUniform(m_Program, i, maxLength, &length, &size, &currentType, nameBuffer);
                nameBuffer[length] = '\0';
                std::string currentName = nameBuffer;
                if (currentName.find("ModelMatrix") == std::string::npos && currentName.find("NormalMatrix") == std::string::npos)
                {
                    m_UniformNamesToTypesMap[std::move(currentName)] = GetTypeFromOpenGLType(currentType);
                }
            }
        }

        delete[] nameBuffer;
    }

    void Shader::Bind() const
    {
        glUseProgram(m_Program);
    }

    void Shader::Unbind() const
    {
        glUseProgram(0);
    }

    int Shader::GetUniformLocation(const std::string& name) const
    {
        return glGetUniformLocation(m_Program, name.c_str());
    }

    int Shader::GetUniformValueInt(const std::string& name) const
    {
        int value;
        glGetUniformiv(m_Program, GetUniformLocation(name), &value);
        return value;
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

    void Shader::SetUniformFloat(const std::string& name, float value) const
    {
        glUniform1f(GetUniformLocation(name), value);
    }

    void Shader::SetUniformInt(const std::string& name, int value) const
    {
        glUniform1i(GetUniformLocation(name), value);
    }
}

#endif
