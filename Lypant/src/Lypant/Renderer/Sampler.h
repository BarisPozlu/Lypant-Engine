#pragma once

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
		ImageWrappingOption WrappingOption;
		ImageFilteringOption FilteringOption;
	};

}