#pragma once

#include <vector>
#include <functional>

namespace lypant
{
	// TODO: storing std::functions for each deletion is not efficent, store only handles later.
	class DeletionQueue
	{
	public:
		DeletionQueue()
		{
			m_Functions.reserve(50);
		}
		inline void PushFunction(std::function<void()>&& function)
		{
			m_Functions.push_back(function);
		}
		inline void Flush()
		{
			for (auto& function : m_Functions)
			{
				function();
			}
			m_Functions.clear();
		}
	private:
		std::vector<std::function<void()>> m_Functions;
	};
}