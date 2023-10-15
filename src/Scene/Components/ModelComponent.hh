#ifndef COMPONENTS_MODEL_COMPONENT_H_
#define COMPONENTS_MODEL_COMPONENT_H_

#include "RenderAPI/EspBuffer.hh"

// libs
#include "glm/glm.hpp"

namespace esp
{
	struct ModelComponent
	{
	 public:
		struct Vertex
		{
			glm::vec3 m_position{};
			glm::vec3 m_color{};
			glm::vec3 m_normal{};
			glm::vec2 m_uv{}; // 2-dim tex coord

			static std::vector<VkVertexInputBindingDescription> get_binding_descriptions();
			static std::vector<VkVertexInputAttributeDescription> get_attribute_descriptions();

			bool operator==(const Vertex& other) const
			{
				return m_position == other.m_position
					&& m_color == other.m_color
					&& m_normal == other.m_normal
					&& m_uv == other.m_uv;
			}
		};

		struct Builder
		{
			std::vector<Vertex> m_vertices{};
			std::vector<uint32_t> m_indices{};

			void loadModel(const std::string& filepath);
		};

	 private:
		EspDevice& m_device;

		std::unique_ptr<EspBuffer> m_vertex_buffer;
		uint32_t m_vertex_count;

		bool m_has_index_buffer{false};
		std::unique_ptr<EspBuffer> m_index_buffer;
		uint32_t m_index_count;

	 public:
		ModelComponent(EspDevice& device, const ModelComponent::Builder& builder);
		~ModelComponent() = default;

		ModelComponent(const ModelComponent&) = delete;
		ModelComponent& operator=(const ModelComponent&) = delete;

		void bind(VkCommandBuffer command_buffer);
		void draw(VkCommandBuffer command_buffer);

	 private:
		void create_vertex_buffer(const std::vector<Vertex>& vertices);
		void create_index_buffer(const std::vector<uint32_t>& indices);
	};
}

#endif //COMPONENTS_MODEL_COMPONENT_H_
