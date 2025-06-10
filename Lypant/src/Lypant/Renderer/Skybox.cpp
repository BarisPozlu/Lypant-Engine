#include "lypch.h"
#include "Skybox.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "Lypant/Util/VertexArrays.h"
#include "Lypant/Util/Util.h"

namespace lypant
{
    Skybox::Skybox(const std::string& path, bool equirectangular)
	{
        m_VertexArray = util::VertexArrays::GetCubemapCube();
        m_Shader = Shader::Load("shaders/Skybox.glsl");

        if (equirectangular)
        {
            m_Cubemap = util::CreateCubemapFromEquirectangularTexture(path);
        }

        else
        {
            m_Cubemap = std::make_shared<Cubemap>(path);
        }
	}
}
