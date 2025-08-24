#pragma once

#include <memory>
#include "Shader.h"
#include "Image.h"

namespace lypant
{
	class Material
	{
	public:
		static std::shared_ptr<Material> Create(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Image>& albedo);
		virtual ~Material() = default;
	};
}
