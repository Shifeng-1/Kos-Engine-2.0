#ifndef MATERIALCOMPONENT_H
#define MATERIALCOMPONENT_H

#include "Component.h"
namespace ecs {
	class MaterialComponent :public Component {
	public:
		std::vector<utility::GUID> materialGUID{};
		REFLECTABLE(MaterialComponent, materialGUID)

	};
}
#endif