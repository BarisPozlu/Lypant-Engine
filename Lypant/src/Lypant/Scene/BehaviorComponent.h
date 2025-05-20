#pragma once

#include "Components.h"

namespace lypant
{
	class BehaviorComponent
	{
	public:
		template<typename T>
		void Bind()
		{
			InstantiateFunction = []() { return static_cast<BehaviorComponentBase*>(new T()); };
			DestroyInstanceFunction = [](BehaviorComponent* component) { delete component->Instance; component->Instance = nullptr; };
		}
	public:
		BehaviorComponentBase* Instance = nullptr;
		BehaviorComponentBase*(*InstantiateFunction)();
		void(*DestroyInstanceFunction)(BehaviorComponent*);
	};
}
