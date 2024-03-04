#ifndef RENDERER_MODEL_MODEL_HH
#define RENDERER_MODEL_MODEL_HH

#include "esppch.hh"

#include "Core/RenderAPI/Resources/EspIndexBuffer.hh"
#include "Core/RenderAPI/Resources/EspTexture.hh"
#include "Core/RenderAPI/Resources/EspVertexBuffer.hh"
#include "Core/RenderAPI/Uniforms/EspUniformManager.hh"

#include "Core/Renderer/Model/Animation/BoneInfo.hh"

#include "Mesh/ModelParts.hh"
#include "Mesh/Vertex.hh"

#include "ModelIterator.hh"
#include "ModelParams.hh"

namespace esp
{
  class Animation;

  class Model
  {
   private:
    std::string m_dir;

    // ---------- FLAGS FOR RENDERER ----------
    bool m_has_many_mesh_nodes;
    // ----------------------------------------

   public:
    ModelParams m_params;

    ModelNode m_root_node;
    std::vector<Mesh> m_meshes;
    std::vector<ModelNode*> m_nodes;

    std::map<std::string, BoneInfo> m_bone_info_map;
    uint32_t m_bone_counter;

    std::vector<std::shared_ptr<Animation>> m_animations;

    std::unique_ptr<EspVertexBuffer> m_vertex_buffer;
    std::unique_ptr<EspIndexBuffer> m_index_buffer;

    static const uint32_t MAX_BONES_PER_VERTEX = 4;

   private:
    void process_node(ModelNode* n_node,
                      aiNode* node,
                      const aiScene* scene,
                      std::vector<uint32_t>& index_buffer,
                      std::vector<Vertex>& vertex_buffer);

    void process_mesh(Mesh& n_mesh,
                      const aiMesh* mesh,
                      const aiScene* scene,
                      std::vector<uint32_t>& index_buffer,
                      std::vector<Vertex>& vertex_buffer);

    void process_mesh_bones(uint32_t vertex_bias,
                            const aiMesh* mesh,
                            const aiScene* scene,
                            std::vector<Vertex>& vertex_buffer);

    void set_renderer_flags();
    void precompute_transform_matrices(ModelNode* node, glm::mat4 prev_matrix);

    std::shared_ptr<Material> load_material(const aiMaterial* ai_material);
    std::shared_ptr<EspTexture> load_material_texture(const aiMaterial* mat, aiTextureType type);

   public:
    PREVENT_COPY(Model)

    Model(const std::string& path_to_model, ModelParams params);
    Model(std::vector<Vertex>& vertex_buffer,
          std::vector<uint32_t>& index_buffer,
          const std::vector<std::shared_ptr<EspTexture>>& textures,
          ModelParams params);
    ~Model();

    inline auto& get_bone_info_map() { return m_bone_info_map; }
    inline uint32_t& get_bone_count() { return m_bone_counter; }
    inline const ModelNode& get_root_node() const { return m_root_node; }

    void set_vertex_buffer(std::vector<Vertex>& vertex_buffer); // Do not use inside layer's update function!
    void set_index_buffer(std::vector<uint32_t>& index_buffer,
                          uint32_t buffer_index); // Do not use inside layer's update function!

    inline bool has_many_mesh_nodes() { return m_has_many_mesh_nodes; }

    ModelIterator begin() { return ModelIterator(this); }
    ModelIterator end() { return ModelIterator(nullptr); }
  };
} // namespace esp

#endif // RENDERER_MODEL_MODEL_HH
