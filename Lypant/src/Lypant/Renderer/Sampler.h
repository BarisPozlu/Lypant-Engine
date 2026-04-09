#pragma once

#include <memory>

namespace lypant
{
	enum class ImageWrappingOption
	{
		Repeat, ClampEdge, ClampBorder
	};

	enum class ImageFilteringOption
	{
		Nearest, Linear
	};

	struct SamplerSpecification
	{
		ImageWrappingOption WrappingOption = ImageWrappingOption::Repeat;
		ImageFilteringOption FilteringOption = ImageFilteringOption::Linear;
	};

	class Sampler
	{
	public:
		static std::shared_ptr<Sampler> Create(const SamplerSpecification& spec);
		virtual ~Sampler() = default;
	};
}
