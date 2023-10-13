#ifndef COMPONENTS_COLOR_COMPONENT_H_
#define COMPONENTS_COLOR_COMPONENT_H_

// libs
#include <glm/glm.hpp>

namespace esp
{
	struct ColorComponent
	{
		glm::vec3 m_color{1.0f};
	};
}

#endif //COMPONENTS_COLOR_COMPONENT_H_