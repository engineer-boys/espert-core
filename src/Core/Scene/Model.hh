#ifndef SCENE_MODEL_HH
#define SCENE_MODEL_HH

#include "esppch.hh"

// Render API
#include "Core/RenderAPI/Pipeline/EspPipelineBuilder.hh"
#include "Core/RenderAPI/Resources/EspIndexBuffer.hh"
#include "Core/RenderAPI/Resources/EspVertexBuffer.hh"

namespace esp
{
  // TODO: move this out of Scene folder
  class Model
  {
   public:
    struct Vertex
    {
      glm::vec3 m_position{};
      glm::vec3 m_color{};
      glm::vec3 m_normal{};
      glm::vec2 m_tex_coord{};

      static EspVertexLayout get_vertex_layout();

      bool operator==(const Vertex& other) const
      {
        return m_position == other.m_position && m_color == other.m_color && m_normal == other.m_normal &&
            m_tex_coord == other.m_tex_coord;
      }
    };

    struct Builder
    {
      std::vector<Vertex> m_vertices{};
      std::vector<uint32_t> m_indices{};

      Builder& load_model(const std::string& filepath);
    };

   private:
    std::unique_ptr<EspVertexBuffer> m_vertex_buffer;

    bool m_has_index_buffer{ false };
    std::unique_ptr<EspIndexBuffer> m_index_buffer;

    bool m_has_instance_buffer{ false };
    std::unique_ptr<EspVertexBuffer> m_instance_buffer;

   public:
    Model(Builder& builder);
    Model(std::vector<Vertex> vertices);
    Model(std::vector<Vertex> vertices, std::vector<uint32_t> indices);

    Model(const Model&)            = delete;
    Model& operator=(const Model&) = delete;

    ~Model() = default;

    void attach();

    inline uint32_t get_vertex_count() { return m_vertex_buffer->get_vertex_count(); }
    inline uint32_t get_index_count() { return m_index_buffer->get_index_count(); }

    template<typename T> void add_instance_buffer(std::vector<T> instances)
    {
      ESP_ASSERT(!m_has_instance_buffer, "Model already has instance buffer")

      m_has_instance_buffer = true;
      m_instance_buffer = EspVertexBuffer::create(instances.data(), sizeof(T), instances.size(), EspBuffer::VISIBLE);
    }
    template<typename T> void update_instance_buffer(std::vector<T> instances, uint32_t offset)
    {
      ESP_ASSERT(m_has_instance_buffer, "Model doesn't have an instance buffer")

      m_instance_buffer->update(instances.data(), sizeof(T), instances.size(), offset);
    }
  };
} // namespace esp

#endif // SCENE_MODEL_HH
