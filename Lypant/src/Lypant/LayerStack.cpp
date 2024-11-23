#include "lypch.h"
#include "LayerStack.h"

namespace lypant
{
	LayerStack::LayerStack() : m_LayerInsert(m_Layers.begin())
	{

	}

	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers)
		{
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_LayerInsert = m_Layers.emplace(m_LayerInsert, layer);
	}

	void LayerStack::PushOverlay(Layer* layer)
	{
		m_Layers.emplace_back(layer);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		std::vector<Layer*>::iterator it = std::find(begin(), end(), layer);

		if (it != end())
		{
			m_Layers.erase(it);
			m_LayerInsert--;
		}

	}

	void LayerStack::PopOverlay(Layer* layer)
	{
		std::vector<Layer*>::iterator it = std::find(begin(), end(), layer);

		if (it != end())
		{
			m_Layers.erase(it);
		}
	}
}
