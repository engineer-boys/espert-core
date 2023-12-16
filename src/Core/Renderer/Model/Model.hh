#ifndef SCENE_MODEL_MODEL_HH
#define SCENE_MODEL_MODEL_HH

#include "Mesh.hh"
#include "esppch.hh"

// Render API
#include "Core/RenderAPI/EspCommandHandler.hh"

// assimp
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace esp
{
  class Model
  {
   public:
    enum EspPostProcessSteps
    {
      EspProcessDefault = 0x0,
      EspProcessFlipUVs = 0x800000
    };

    struct Builder
    {
      std::vector<std::shared_ptr<Mesh>> m_meshes;
      std::string m_dir;

      Builder& load_model(const std::string& filepath, unsigned int p_flags = EspProcessDefault);

     private:
      void process_node(aiNode* node, const aiScene* scene);
      void process_mesh(aiMesh* mesh, const aiScene* scene);
      std::vector<std::shared_ptr<EspTexture>> load_material_textures(aiMaterial* mat, aiTextureType type);
    };

   private:
    std::vector<std::shared_ptr<Mesh>> m_meshes;

    bool m_has_instance_buffer{ false };
    std::unique_ptr<EspVertexBuffer> m_instance_buffer;

   public:
    Model(Builder& builder, EspPipeline& pipeline); // TODO: add default pipeline argument when it's ready
    Model(std::shared_ptr<Mesh> mesh,               //
          std::vector<std::shared_ptr<EspTexture>> textures,
          EspPipeline& pipeline);

    PREVENT_COPY(Model)

    ~Model() = default;

    void draw(); // TODO: this will be called only through renderer - make it private

    inline const std::vector<std::shared_ptr<Mesh>>& get_meshes() { return m_meshes; }
    inline const uint32_t get_meshes_count() const { return m_meshes.size(); }

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

   private:
    void add_materials(EspPipeline& pipeline);
  };
} // namespace esp

#endif // SCENE_MODEL_MODEL_HH
