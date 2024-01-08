#ifndef SCENE_MODEL_MODEL_HH
#define SCENE_MODEL_MODEL_HH

#include "Mesh.hh"
#include "esppch.hh"

// Render API
#include "Core/RenderAPI/Work/EspJob.hh"

namespace esp
{
  enum EspPostProcessSteps
  {
    EspProcessDefault   = 0x0,
    EspProcessFlipUVs   = 0x800000,
    EspCaclTangentSpace = 0x1,
  };

  struct ModelParams
  {
    unsigned int p_flags                       = EspProcessDefault;
    bool load_material                         = true;
    std::vector<MaterialTextureLayout> layouts = { { 1, 0, EspTextureType::ALBEDO },
                                                   { 1, 1, EspTextureType::NORMAL },
                                                   { 1, 2, EspTextureType::METALLIC },
                                                   { 1, 3, EspTextureType::ROUGHNESS },
                                                   { 1, 4, EspTextureType::AO } };
  };

  class Model
  {
   public:
    struct Builder
    {
      std::vector<std::shared_ptr<Mesh>> m_meshes;
      std::string m_dir;
      std::shared_ptr<EspShader> m_shader = ShaderSystem::get_default_shader();

      Builder& load_model(const std::string& filepath, const ModelParams& params = {});
      Builder& set_shader(std::shared_ptr<EspShader> shader);

     private:
      void process_node(aiNode* node, const aiScene* scene, const ModelParams& params);
      void process_mesh(aiMesh* mesh, const aiScene* scene, const ModelParams& params);
      std::shared_ptr<EspTexture> load_material_texture(aiMaterial* mat, aiTextureType type);
    };

   private:
    std::vector<std::shared_ptr<Mesh>> m_meshes;

    bool m_has_instance_buffer{ false };
    std::unique_ptr<EspVertexBuffer> m_instance_buffer;

   public:
    Model(Builder& builder);
    Model(std::shared_ptr<Mesh> mesh);

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
  };
} // namespace esp

#endif // SCENE_MODEL_MODEL_HH
