#include <utility>

#include "Mesh.hh"

namespace esp
{
  EspVertexLayout Mesh::Vertex::get_vertex_layout()
  {
    return VTX_LAYOUT(sizeof(Mesh::Vertex),
                      0,
                      ESP_VERTEX_INPUT_RATE_VERTEX,
                      ATTR(0, ESP_FORMAT_R32G32B32_SFLOAT, offsetof(Mesh::Vertex, m_position)),
                      ATTR(1, ESP_FORMAT_R32G32B32_SFLOAT, offsetof(Mesh::Vertex, m_color)),
                      ATTR(2, ESP_FORMAT_R32G32B32_SFLOAT, offsetof(Mesh::Vertex, m_normal)),
                      ATTR(3, ESP_FORMAT_R32G32_SFLOAT, offsetof(Mesh::Vertex, m_tex_coord)));
  }

  Mesh::Mesh(std::vector<Vertex> vertices)
  {
    m_vertex_buffer = EspVertexBuffer::create(vertices.data(), sizeof(Vertex), vertices.size());
  }

  Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices)
  {
    m_vertex_buffer = EspVertexBuffer::create(vertices.data(), sizeof(Vertex), vertices.size());

    auto index_count   = indices.size();
    m_has_index_buffer = index_count > 0;
    if (m_has_index_buffer) { m_index_buffer = EspIndexBuffer::create(indices.data(), index_count); }
  }

  Mesh::Mesh(std::vector<Vertex> vertices,
             std::vector<uint32_t> indices,
             std::vector<std::shared_ptr<EspTexture>> textures)
  {
    m_vertex_buffer = EspVertexBuffer::create(vertices.data(), sizeof(Vertex), vertices.size());

    auto index_count   = indices.size();
    m_has_index_buffer = index_count > 0;
    if (m_has_index_buffer) { m_index_buffer = EspIndexBuffer::create(indices.data(), index_count); }

    m_textures = std::move(textures);
  }

  void Mesh::draw()
  {
    m_vertex_buffer->attach();
    if (m_has_material) { m_material->attach(); }
    if (m_has_index_buffer)
    {
      m_index_buffer->attach();
      EspCommandHandler::draw_indexed(get_index_count());
    }
    else { EspCommandHandler::draw(get_vertex_count()); }
  }

  void Mesh::draw(esp::EspVertexBuffer& instance_buffer)
  {
    m_vertex_buffer->attach_instanced(instance_buffer);
    if (m_has_material) { m_material->attach(); }
    if (m_has_index_buffer)
    {
      m_index_buffer->attach();
      EspCommandHandler::draw_indexed(get_index_count(), instance_buffer.get_vertex_count());
    }
    else { EspCommandHandler::draw(get_vertex_count(), instance_buffer.get_vertex_count()); }
  }

  void Mesh::add_material(esp::EspPipeline& pipeline)
  {
    m_has_material = true;
    m_material     = Material::create(pipeline, m_textures);
    m_textures.clear();
  }
} // namespace esp