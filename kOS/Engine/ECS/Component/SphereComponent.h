#ifndef SPHERECOMPONENT_H
#define SPHERECOMPONENT_H

#include "Component.h";
namespace ecs {
	class SphereRendererComponent :public Component {
	public:

		REFLECTABLE(SphereRendererComponent);
	};
}

#endif