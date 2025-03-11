#include "lypch.h"
#include "Skybox.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

namespace lypant
{
	Skybox::Skybox(const std::string& path)
	{
        m_VertexArray = std::make_shared<VertexArray>();

        // TODO: Update the vertex data not to contain duplicate vertices (didn't want to bother)
        float vertexData[]
        {
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        };

        std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>(vertexData, sizeof(vertexData));

        BufferLayout layout
        {
            {ShaderDataType::Float3, "a_Position"}
        };

        vertexBuffer->SetLayout(layout);

        m_VertexArray->AddVertexBuffer(vertexBuffer);

        unsigned int indexData[36];
        for (int i = 0; i < 36; i++)
        {
            indexData[i] = i;
        }

        std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBuffer>(indexData, 36);
        m_VertexArray->SetIndexBuffer(indexBuffer);

        m_Shader = Shader::Load("shaders/Skybox.glsl");
        m_Cubemap = std::make_unique<Cubemap>(path);
	}
}
