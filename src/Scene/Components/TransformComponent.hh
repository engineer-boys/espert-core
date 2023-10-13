#ifndef COMPONENTS_TRANSFORM_COMPONENT_H_
#define COMPONENTS_TRANSFORM_COMPONENT_H_

// libs
#include <glm/glm.hpp>

namespace esp
{
	struct TransformComponent
	{
		glm::vec3 m_translation{}; // (position offset)
		glm::vec3 m_scale{ 1.0f };
		glm::vec3 m_rotation{};

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation, const glm::vec3& scale, const glm::vec3& rotation)
			: m_translation{translation}, m_scale{scale}, m_rotation{rotation} {}

		// transformMat = translate * Ry * Rx * Rz * scale
		// m_rotation correspond to Tait-bryan angles of Y(1), X(2), Z(3)
		glm::mat4 get_transform_mat();
		glm::mat3 get_normal_transform_mat();
	};

}

#endif //COMPONENTS_TRANSFORM_COMPONENT_H_