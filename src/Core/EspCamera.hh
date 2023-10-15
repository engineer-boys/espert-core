#ifndef CORE_ESP_CAMERA_H_
#define CORE_ESP_CAMERA_H_

#include "Scene/Components/Components.hh"

// libs
#include <glm/glm.hpp>

namespace esp
{
	class EspCamera
	{
	 private:
		glm::mat4 m_projection_mat{1.0f};
		glm::mat4 m_view_mat{1.0f};        // world -> camera
		glm::mat4 m_inverse_view_mat{1.0f}; // camera -> world

		float m_fov{glm::radians(50.0f)};
		float m_near{0.1f};
		float m_far{100.0f};

	 public:
		TransformComponent m_transform{};

		EspCamera() = default;
		~EspCamera() = default;

		EspCamera(const EspCamera&) = delete;
		EspCamera& operator=(const EspCamera&) = delete;
		EspCamera(EspCamera&&) = default;
		EspCamera& operator=(EspCamera&&) = default;

		void set_orthographic(float left, float right, float top, float bottom, float near, float far);
		void set_perspective(float fov, float aspect_ratio, float near, float far);
		inline void set_perspective(float aspect_ratio)
		{
			set_perspective(m_fov, aspect_ratio, m_near, m_far);
		}

		inline void set_fov(float val) { m_fov = glm::radians(val); }
		inline void set_near(float val) { m_near = val; }
		inline void set_far(float val)
		{
			m_far = val;
		}

		void set_view_dir(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{0.0f, -1.0f, 0.0f});
		inline void set_view_target( glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{0.0f, -1.0f, 0.0f})
		{
			set_view_dir(position, target - position, up);
		}
		void set_view(glm::vec3 position, glm::vec3 rotation);

		inline void update_view()
		{
			set_view(m_transform.m_translation, m_transform.m_rotation);
 		}

		inline float get_fov() const { return m_fov; }
		inline float get_near() const { return m_near; }
		inline float get_far() const { return m_far; }
		inline const glm::mat4& get_projection_mat() const { return m_projection_mat; }
		inline const glm::mat4& get_view_mat() const { return m_view_mat; }
		inline const glm::mat4& get_inverse_view_mat() const { return m_inverse_view_mat; }
		inline const glm::vec3 get_position() const { return glm::vec3(m_inverse_view_mat[3]); }
	};
}

#endif //CORE_ESP_CAMERA_H_
