#ifndef SCENE_MODEL_MESH_HH
#define SCENE_MODEL_MESH_HH

#include "esppch.hh"

// Render API
#include "Core/RenderAPI/EspCommandHandler.hh"
#include "Core/RenderAPI/Pipeline/EspPipelineBuilder.hh"
#include "Core/RenderAPI/Resources/EspIndexBuffer.hh"
#include "Core/RenderAPI/Resources/EspTexture.hh"
#include "Core/RenderAPI/Resources/EspVertexBuffer.hh"

#include "Core/Resources/Systems/MaterialSystem.hh"

namespace esp
{
  class Model;

  class Mesh
  {
   public:
    struct Vertex
    {
      glm::vec3 m_position{};
      glm::vec3 m_color{};
      glm::vec3 m_normal{};
      glm::vec2 m_tex_coord{};

      static EspVertexLayout get_vertex_layout();
    };

   private:
    std::unique_ptr<EspVertexBuffer> m_vertex_buffer;

    bool m_has_index_buffer{ false };
    std::unique_ptr<EspIndexBuffer> m_index_buffer;

    std::vector<std::shared_ptr<EspTexture>> m_textures; // TODO: Replace it with material class
    bool m_has_material{ false };
    std::shared_ptr<Material> m_material;

   public:
    Mesh(std::vector<Vertex> vertices);
    Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices);
    Mesh(std::vector<Vertex> vertices,
         std::vector<uint32_t> indices,
         std::vector<std::shared_ptr<EspTexture>> textures);

    Mesh(Mesh&& other) noexcept = default;
    PREVENT_COPY(Mesh)

    ~Mesh() = default;

    inline uint32_t get_vertex_count() { return m_vertex_buffer->get_vertex_count(); }
    inline uint32_t get_index_count() { return m_index_buffer->get_index_count(); }

   private:
    void draw();
    void draw(EspVertexBuffer& instance_buffer);

    void add_material(EspPipeline& pipeline);

    friend class Model;
  };
} // namespace esp

#endif // SCENE_MODEL_MESH_HH
