#include "ModelParams.hh"

namespace esp
{
  EspVertexLayout ModelParams::get_vertex_layouts(bool instancing) const
  {
    auto input_rate = EspVertexInputRate::ESP_VERTEX_INPUT_RATE_VERTEX;
    if (instancing) { input_rate = EspVertexInputRate::ESP_VERTEX_INPUT_RATE_INSTANCE; }

    uint32_t binding = 0;

    uint32_t size = 0;
    size += m_position ? Vertex::size_of_position() : 0;
    size += m_color ? Vertex::size_of_color() : 0;
    size += m_normal ? Vertex::size_of_normal() : 0;
    size += m_tex_coord ? Vertex::size_of_tex_coord() : 0;
    size += m_bone_ids ? Vertex::size_of_bone_ids() : 0;
    size += m_weights ? Vertex::size_of_weights() : 0;
    size += m_tangent ? Vertex::size_of_tangent() : 0;

    uint32_t offset   = 0;
    uint32_t location = 0;
    std::vector<EspVertexAttribute> attr;

    if (m_position)
    {
      attr.push_back(EspVertexAttribute(location, ESP_FORMAT_R32G32B32_SFLOAT, offset));

      location += 1;
      offset += Vertex::size_of_position();
    }

    if (m_color)
    {
      attr.push_back(EspVertexAttribute(location, ESP_FORMAT_R32G32B32_SFLOAT, offset));

      location += 1;
      offset += Vertex::size_of_color();
    }

    if (m_normal)
    {
      attr.push_back(EspVertexAttribute(location, ESP_FORMAT_R32G32B32_SFLOAT, offset));

      location += 1;
      offset += Vertex::size_of_normal();
    }

    if (m_tex_coord)
    {
      attr.push_back(EspVertexAttribute(location, ESP_FORMAT_R32G32_SFLOAT, offset));

      location += 1;
      offset += Vertex::size_of_tex_coord();
    }

    if (m_bone_ids)
    {
      attr.push_back(EspVertexAttribute(location, ESP_FORMAT_R32G32B32A32_SINT, offset));

      location += 1;
      offset += Vertex::size_of_bone_ids();
    }

    if (m_weights)
    {
      attr.push_back(EspVertexAttribute(location, ESP_FORMAT_R32G32B32A32_SFLOAT, offset));

      location += 1;
      offset += Vertex::size_of_weights();
    }

    if (m_tangent)
    {
      attr.push_back(EspVertexAttribute(location, ESP_FORMAT_R32G32B32_SFLOAT, offset));

      location += 1;
      offset += Vertex::size_of_tangent();
    }

    return EspVertexLayout(size, binding, input_rate, attr);
  }

  void ModelParams::parse_to_vertex_byte_buffer(std::vector<Vertex>& vertex_buffer,
                                                std::vector<uint8_t>& vertex_byte_buffer) const
  {
    for (auto& vert : vertex_buffer)
    {
      // position
      if (m_position)
      {
        for (int idx = 0; idx < 3; ++idx)
        {
          float xyz    = vert.m_position[idx];
          uint8_t* ptr = reinterpret_cast<uint8_t*>(&xyz);

          for (int i = 0; i < sizeof(float); i++)
          {
            vertex_byte_buffer.push_back(ptr[i]);
          }
        }
      }

      // color
      if (m_color)
      {
        for (int idx = 0; idx < 3; ++idx)
        {
          float xyz    = vert.m_color[idx];
          uint8_t* ptr = reinterpret_cast<uint8_t*>(&xyz);

          for (int i = 0; i < sizeof(float); i++)
          {
            vertex_byte_buffer.push_back(ptr[i]);
          }
        }
      }

      // normal
      if (m_normal)
      {
        for (int idx = 0; idx < 3; ++idx)
        {
          float xyz    = vert.m_normal[idx];
          uint8_t* ptr = reinterpret_cast<uint8_t*>(&xyz);

          for (int i = 0; i < sizeof(float); i++)
          {
            vertex_byte_buffer.push_back(ptr[i]);
          }
        }
      }

      // text coords
      if (m_tex_coord)
      {
        for (int idx = 0; idx < 2; ++idx)
        {
          float xy     = vert.m_tex_coord[idx];
          uint8_t* ptr = reinterpret_cast<uint8_t*>(&xy);

          for (int i = 0; i < sizeof(float); i++)
          {
            vertex_byte_buffer.push_back(ptr[i]);
          }
        }
      }

      // bone ids
      if (m_bone_ids)
      {
        for (int idx = 0; idx < 4; ++idx)
        {
          int xyza     = vert.m_bone_ids[idx];
          uint8_t* ptr = reinterpret_cast<uint8_t*>(&xyza);

          for (int i = 0; i < sizeof(int); i++)
          {
            vertex_byte_buffer.push_back(ptr[i]);
          }
        }
      }

      // weights
      if (m_weights)
      {
        for (int idx = 0; idx < 4; ++idx)
        {
          float xyza   = vert.m_weights[idx];
          uint8_t* ptr = reinterpret_cast<uint8_t*>(&xyza);

          for (int i = 0; i < sizeof(float); i++)
          {
            vertex_byte_buffer.push_back(ptr[i]);
          }
        }
      }

      // tangent
      if (m_tangent)
      {
        for (int idx = 0; idx < 3; ++idx)
        {
          float xyz    = vert.m_tangent[idx];
          uint8_t* ptr = reinterpret_cast<uint8_t*>(&xyz);

          for (int i = 0; i < sizeof(float); i++)
          {
            vertex_byte_buffer.push_back(ptr[i]);
          }
        }
      }
    }
  }
} // namespace esp
