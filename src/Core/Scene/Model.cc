#include "Model.hh"

// tiny_obj_loader
#include "tiny_obj_loader.h"

namespace std
{
  template<> struct hash<esp::Model::Vertex>
  {
    size_t operator()(const esp::Model::Vertex& vertex) const
    {
      size_t seed = 0;
      esp::hashCombine(seed, vertex.m_position, vertex.m_color, vertex.m_normal, vertex.m_tex_coord);
      return seed;
    }
  };
} // namespace std

namespace esp
{
  EspVertexLayout Model::Vertex::get_vertex_layout()
  {
    return VTX_LAYOUT(sizeof(Model::Vertex),
                      0,
                      ESP_VERTEX_INPUT_RATE_VERTEX,
                      ATTR(0, ESP_FORMAT_R32G32B32_SFLOAT, offsetof(Model::Vertex, m_position)),
                      ATTR(1, ESP_FORMAT_R32G32B32_SFLOAT, offsetof(Model::Vertex, m_color)),
                      ATTR(2, ESP_FORMAT_R32G32B32_SFLOAT, offsetof(Model::Vertex, m_normal)),
                      ATTR(3, ESP_FORMAT_R32G32_SFLOAT, offsetof(Model::Vertex, m_tex_coord)));
  }

  Model::Builder& Model::Builder::load_model(const std::string& filepath)
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
          vertex.m_position = { attrib.vertices[3 * index.vertex_index + 0],
                                attrib.vertices[3 * index.vertex_index + 1],
                                attrib.vertices[3 * index.vertex_index + 2] };

          vertex.m_color = { attrib.colors[3 * index.vertex_index + 0],
                             attrib.colors[3 * index.vertex_index + 1],
                             attrib.colors[3 * index.vertex_index + 2] };
        }
        if (index.normal_index >= 0)
        {
          vertex.m_normal = { attrib.normals[3 * index.normal_index + 0],
                              attrib.normals[3 * index.normal_index + 1],
                              attrib.normals[3 * index.normal_index + 2] };
        }
        if (index.texcoord_index >= 0)
        {
          vertex.m_tex_coord = {
            attrib.texcoords[2 * index.texcoord_index + 0],
            // TODO: make calculating tex coord platform agnostic
            1.0f - attrib.texcoords[2 * index.texcoord_index + 1], // vertical texture component has to be flipped in
                                                                   // Vulkan because of it's coordinate system
            //
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

    return *this;
  }

  Model::Model(Builder& builder)
  {
    m_vertex_buffer = EspVertexBuffer::create(builder.m_vertices.data(), sizeof(Vertex), builder.m_vertices.size());

    auto index_count   = builder.m_indices.size();
    m_has_index_buffer = index_count > 0;
    if (m_has_index_buffer) { m_index_buffer = EspIndexBuffer::create(builder.m_indices.data(), index_count); }
  }

  Model::Model(std::vector<Vertex> vertices)
  {
    m_vertex_buffer = EspVertexBuffer::create(vertices.data(), sizeof(Vertex), vertices.size());
  }

  Model::Model(std::vector<Vertex> vertices, std::vector<uint32_t> indices)
  {
    m_vertex_buffer = EspVertexBuffer::create(vertices.data(), sizeof(Vertex), vertices.size());

    auto index_count   = indices.size();
    m_has_index_buffer = index_count > 0;
    if (m_has_index_buffer) { m_index_buffer = EspIndexBuffer::create(indices.data(), index_count); }
  }

  void Model::attach()
  {
    if (m_has_instance_buffer) { m_vertex_buffer->attach_instanced(*m_instance_buffer); }
    else { m_vertex_buffer->attach(); }
    if (m_has_index_buffer) { m_index_buffer->attach(); }
  }
} // namespace esp