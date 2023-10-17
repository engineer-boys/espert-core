#include "ModelComponent.hh"

// libs
#define TINYOBJLOADER_IMPLEMENTATION
#include "external/tinyobjloader/tiny_obj_loader.h"


// std
#include <cassert>
#include <unordered_map>

using namespace esp;

namespace std
{
	template<>
	struct hash<esp::ModelComponent::Vertex>
	{
		size_t operator()(const esp::ModelComponent::Vertex& vertex) const
		{
			size_t seed = 0;
			esp::hashCombine(seed, vertex.m_position, vertex.m_color, vertex.m_normal, vertex.m_uv);
			return seed;
		}
	};
}

std::vector<VkVertexInputBindingDescription> ModelComponent::Vertex::get_binding_descriptions()
{
	std::vector<VkVertexInputBindingDescription> binding_descriptions(1);
	binding_descriptions[0].binding = 0;
	binding_descriptions[0].stride = sizeof(Vertex);
	binding_descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return binding_descriptions;
}

std::vector<VkVertexInputAttributeDescription> ModelComponent::Vertex::get_attribute_descriptions()
{
	std::vector<VkVertexInputAttributeDescription> attribute_descriptions{};

	attribute_descriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, m_position) });
	attribute_descriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, m_color) });
	attribute_descriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, m_normal) });
	attribute_descriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, m_uv) });

	return attribute_descriptions;
}

void ModelComponent::Builder::loadModel(const std::string& filepath)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str()))
	{
		ESP_CORE_ERROR("Failed to load object");
		throw std::runtime_error(warn + err);
	}

	m_vertices.clear();
	m_indices.clear();

	std::unordered_map<Vertex, uint32_t> unique_vertices{};

	for (auto& shape : shapes)
	{
		for (auto& index : shape.mesh.indices)
		{
			Vertex vertex{};

			if (index.vertex_index >= 0)
			{
				vertex.m_position =
					{
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2]
					};

				vertex.m_color =
					{
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2]
					};
			}
			if (index.normal_index >= 0)
			{
				vertex.m_normal =
					{
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]
					};
			}
			if (index.texcoord_index >= 0)
			{
				vertex.m_uv =
					{
						attrib.texcoords[3 * index.texcoord_index + 0],
						attrib.texcoords[3 * index.texcoord_index + 1],
					};
			}

			if (unique_vertices.count(vertex) == 0)
			{
				unique_vertices[vertex] = static_cast<uint32_t>(m_vertices.size());
				m_vertices.push_back(vertex);
			}
			m_indices.push_back(unique_vertices[vertex]);
		}
	}
}

ModelComponent::ModelComponent(EspDevice& device, const ModelComponent::Builder& builder)
	: m_device{ device }
{
	create_vertex_buffer(builder.m_vertices);
	create_index_buffer(builder.m_indices);
}

ModelComponent::ModelComponent(EspDevice& device, const std::vector<Vertex>& vertices)
	: m_device{ device }
{
	create_vertex_buffer(vertices);
}

void ModelComponent::create_vertex_buffer(const std::vector<Vertex>& vertices)
{
	m_vertex_count = static_cast<uint32_t>(vertices.size());
	assert(m_vertex_count >= 3 && "Vertex count must be at least 3");

	VkDeviceSize buffer_size = sizeof(vertices[0]) * m_vertex_count;
	uint32_t vertex_size = sizeof(vertices[0]);

	EspBuffer staging_buffer
		{
			m_device,
			vertex_size,
			m_vertex_count,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			// host (cpu) has visible data | vertex buffer data = device (gpu) memory
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

	staging_buffer.map();
	staging_buffer.write_to_buffer((void*)vertices.data());

	m_vertex_buffer = std::make_unique<EspBuffer>(
		m_device,
		vertex_size,
		m_vertex_count,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	m_device.copy_buffer(staging_buffer.get_buffer(), m_vertex_buffer->get_buffer(), buffer_size);
}

void ModelComponent::create_index_buffer(const std::vector<uint32_t>& indices)
{
	m_index_count = static_cast<uint32_t>(indices.size());
	m_has_index_buffer = m_index_count > 0;

	if (!m_has_index_buffer) return;

	VkDeviceSize buffer_size = sizeof(indices[0]) * m_index_count;
	uint32_t index_size = sizeof(indices[0]);

	EspBuffer staging_buffer{
		m_device,
		index_size,
		m_index_count,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		// host (cpu) has visible data | vertex buffer data = device (gpu) memory
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
	};

	staging_buffer.map();
	staging_buffer.write_to_buffer((void*)indices.data());

	m_index_buffer = std::make_unique<EspBuffer>(
		m_device,
		index_size,
		m_index_count,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	m_device.copy_buffer(staging_buffer.get_buffer(), m_index_buffer->get_buffer(), buffer_size);
}

void ModelComponent::bind(VkCommandBuffer command_buffer)
{
	VkBuffer buffers[] = { m_vertex_buffer->get_buffer() };
	VkDeviceSize offsets[] = { 0 };

	vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers, offsets);

	if (m_has_index_buffer)
	{
		vkCmdBindIndexBuffer(command_buffer, m_index_buffer->get_buffer(), 0, VK_INDEX_TYPE_UINT32);
	}
}
void ModelComponent::draw(VkCommandBuffer command_buffer)
{
	if (m_has_index_buffer)
	{
		vkCmdDrawIndexed(command_buffer, m_index_count, 1, 0, 0, 0);
	}
	else
	{
		vkCmdDraw(command_buffer, m_vertex_count, 1, 0, 0);
	}
}