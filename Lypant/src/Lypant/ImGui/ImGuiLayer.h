#pragma once

#include "Lypant/Core/Layer.h"

namespace lypant
{
	class ImGuiLayer : public Layer
	{
	public:
		static ImGuiLayer* Create();
		virtual ~ImGuiLayer() = default;
	protected:
		ImGuiLayer() = default;
	public:
		virtual void Begin() = 0;
		virtual void End() = 0;
	};
}
