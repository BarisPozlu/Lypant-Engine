#include "lypch.h"
#include "Samplers.h"
#include "Lypant/Renderer/Sampler.h"

namespace lypant
{
	namespace util
	{
		void Samplers::Create()
		{
			SamplerSpecification spec;
			spec.FilteringOption = ImageFilteringOption::Linear;

			spec.WrappingOption = ImageWrappingOption::Repeat;
			s_LinearRepeat = Sampler::Create(spec);

			spec.WrappingOption = ImageWrappingOption::ClampEdge;
			s_LinearClampEdge = Sampler::Create(spec);
		}

		void Samplers::Destroy()
		{
			s_LinearRepeat.reset();
			s_LinearClampEdge.reset();
		}
	}
}