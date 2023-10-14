#ifndef COMPONENTS_MODEL_COMPONENT_H_
#define COMPONENTS_MODEL_COMPONENT_H_

#include "RenderAPI/EspBuffer.hh"

// libs
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace esp
{
	struct ModelComponent
	{
	 public:
		struct Vertex
		{
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{}; // 2-dim tex coord

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex& other) const
			{
				return position == other.position
					&& color == other.color
					&& normal == other.normal
					&& uv == other.uv;
			}
		};

		struct Builder
		{
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string& filepath);
		};

	 private:
		EspDevice& espDevice;

		std::unique_ptr<EspBuffer> vertexBuffer;
		uint32_t vertexCount;

		bool hasIndexBuffer{ false };
		std::unique_ptr<EspBuffer> indexBuffer;
		uint32_t indexCount;

	 public:
		ModelComponent(EspDevice& device, const ModelComponent::Builder& builder);
		~ModelComponent() = default;

		ModelComponent(const ModelComponent&) = delete;
		ModelComponent& operator=(const ModelComponent&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	 private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);
	};
}

#endif //COMPONENTS_MODEL_COMPONENT_H_
