#ifndef RENDERER_NMODEL_MODEL_HH
#define RENDERER_NMODEL_MODEL_HH

#include "esppch.hh"

#include "Core/RenderAPI/Resources/EspIndexBuffer.hh"
#include "Core/RenderAPI/Resources/EspVertexBuffer.hh"
#include "Core/RenderAPI/Uniforms/EspUniformManager.hh"

#include "Core/Renderer/Model3/Animation/BoneInfo.hh"

#include "Mesh/ModelParts.hh"
#include "Mesh/NVertex.hh"

#include "NModelIterator.hh"

namespace esp
{
  class Animation;

  class NModel
  {
   private:
   public:
    NModelParams m_params;

    NNode m_root_node;
    std::vector<NMesh> m_meshes;
    std::vector<NNode*> m_nodes;

    std::map<std::string, BoneInfo> m_bone_info_map;
    uint32_t m_bone_counter;

    std::vector<std::shared_ptr<Animation>> m_animations;

    std::unique_ptr<EspVertexBuffer> m_vertex_buffer;
    std::unique_ptr<EspIndexBuffer> m_index_buffer;

   private:
    void process_node(NNode* n_node,
                      aiNode* node,
                      const aiScene* scene,
                      std::vector<uint32_t>& index_buffer,
                      std::vector<NVertex>& vertex_buffer);

    void process_mesh(NMesh& n_mesh,
                      const aiMesh* mesh,
                      const aiScene* scene,
                      std::vector<uint32_t>& index_buffer,
                      std::vector<NVertex>& vertex_buffer);

    void process_mesh_bones(uint32_t vertex_bias,
                            const aiMesh* mesh,
                            const aiScene* scene,
                            std::vector<NVertex>& vertex_buffer);

    void precompute_transform_matrices(NNode* node, glm::mat4 prev_matrix);

   public:
    PREVENT_COPY(NModel)

    NModel(std::string path_to_model, NModelParams params);
    ~NModel();

    inline auto& get_bone_info_map() { return m_bone_info_map; }
    inline uint32_t& get_bone_count() { return m_bone_counter; }
    inline const NNode& get_root_node() const { return m_root_node; }

    void set_localisation(glm::mat4 localisation = glm::mat4(1));

    void draw(std::vector<std::unique_ptr<EspUniformManager>>& u_manager,
              std::vector<std::unique_ptr<EspUniformManager>>& u_anim_manager);

    void drawNode(NNode* node,
                  glm::mat4 prev_trans,
                  std::vector<std::unique_ptr<EspUniformManager>>& u_manager,
                  std::vector<std::unique_ptr<EspUniformManager>>& u_anim_manager);

    NModelIterator begin() { return NModelIterator(this); }
    NModelIterator end() { return NModelIterator(nullptr); }
  };
} // namespace esp

#endif // RENDERER_NMODEL_MODEL_HH
