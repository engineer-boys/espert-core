#include "Model.hh"

#include "Core/Resources/Systems/ResourceSystem.hh"
#include "Core/Resources/Systems/TextureSystem.hh"

#include "Core/RenderAPI/Work/EspJob.hh"

#include "Core/Renderer/Model/Animation/Animation.hh"
#include "Core/Renderer/Utils/AssimpUtils.hh"

#include <map>

namespace esp
{
  std::shared_ptr<Material> Model::load_material(const aiMaterial* ai_material)
  {
    std::vector<std::shared_ptr<EspTexture>> textures;

    for (const auto& layout : m_params.m_material_texture_layout)
    {
      auto texture = load_material_texture(ai_material, esp_texture_type_to_assimp(layout.type));
      if (texture) textures.push_back(texture);
    }

    return MaterialSystem::acquire(textures, m_params.m_material_texture_layout);
  }

  std::shared_ptr<EspTexture> Model::load_material_texture(const aiMaterial* mat, aiTextureType type)
  {
    for (uint32_t i = 0; i < mat->GetTextureCount(type); i++)
    {
      aiString str;
      mat->GetTexture(type, i, &str);
      TextureParams params;
      auto file_path = (fs::path(m_dir) / fs::path(str.C_Str())).string();

      return TextureSystem::acquire(file_path, params);
    }

    return nullptr;
  }

  void Model::set_renderer_flags()
  {
    m_has_many_mesh_nodes =
        std::count_if(m_nodes.begin(), m_nodes.end(), [](ModelNode* node) { return node->m_has_meshes; }) > 1;
  }

  void Model::precompute_transform_matrices(ModelNode* node, glm::mat4 prev_matrix)
  {
    node->m_precomputed_transformation = prev_matrix * node->m_transformation;

    for (auto child : node->m_children)
    {
      precompute_transform_matrices(m_nodes[child], node->m_precomputed_transformation);
    }
  }

  void Model::process_mesh_bones(uint32_t vertex_bias,
                                 const aiMesh* mesh,
                                 const aiScene* scene,
                                 std::vector<Vertex>& vertex_buffer)
  {
    auto vertex_weights = std::vector<uint32_t>(vertex_buffer.size() - vertex_bias, 0);

    for (uint32_t bone_idx = 0; bone_idx < mesh->mNumBones; ++bone_idx)
    {
      int32_t bone_id       = -1;
      std::string bone_name = mesh->mBones[bone_idx]->mName.C_Str();

      if (m_bone_info_map.find(bone_name) == m_bone_info_map.end())
      {
        BoneInfo new_bone_info = {};
        new_bone_info.m_id     = static_cast<uint32_t>(m_bone_counter);
        new_bone_info.m_bone_space_matrix =
            AssimpUtils::convert_assimp_mat_to_glm_mat4(mesh->mBones[bone_idx]->mOffsetMatrix);

        m_bone_info_map[bone_name] = new_bone_info;
        bone_id                    = m_bone_counter;
        m_bone_counter++;
      }
      else { bone_id = m_bone_info_map[bone_name].m_id; }

      auto weights             = mesh->mBones[bone_idx]->mWeights;
      uint32_t weights_counter = mesh->mBones[bone_idx]->mNumWeights;

      for (int weight_idx = 0; weight_idx < weights_counter; ++weight_idx)
      {
        uint32_t vertex_id = weights[weight_idx].mVertexId;
        float weight       = weights[weight_idx].mWeight;

        Vertex& vertex = vertex_buffer[vertex_bias + vertex_id];

        if (vertex_weights[vertex_id] < MAX_BONES_PER_VERTEX)
        {
          uint32_t& index = vertex_weights[vertex_id];

          if (m_params.m_weights) { vertex.m_weights[index] = weight; }
          if (m_params.m_bone_ids) { vertex.m_bone_ids[index] = bone_id; }

          index++;
        }
        else { throw std::runtime_error("Espert doesn't support animation with more than 4 bones per vertex!"); }
      }
    }
  }

  void Model::process_mesh(Mesh& n_mesh,
                           const aiMesh* mesh,
                           const aiScene* scene,
                           std::vector<uint32_t>& index_buffer,
                           std::vector<Vertex>& vertex_buffer)
  {
    uint32_t vertex_bias = static_cast<uint32_t>(vertex_buffer.size());

    n_mesh.m_first_index = static_cast<uint32_t>(index_buffer.size());

    for (uint32_t vert_idx = 0; vert_idx < mesh->mNumVertices; vert_idx++)
    {
      Vertex vertex = {};

      if (m_params.m_position && mesh->HasPositions())
      {
        vertex.m_position = AssimpUtils::convert_assimp_vec_to_glm_vec3(mesh->mVertices[vert_idx]);
      }

      if (m_params.m_normal && mesh->HasNormals())
      {
        vertex.m_normal = AssimpUtils::convert_assimp_vec_to_glm_vec3(mesh->mNormals[vert_idx]);
      }

      if (m_params.m_tex_coord)
      {
        vertex.m_tex_coord = glm::vec2{ mesh->mTextureCoords[0][vert_idx].x, mesh->mTextureCoords[0][vert_idx].y };
      }

      if (m_params.m_tangent && mesh->HasTangentsAndBitangents())
      {
        vertex.m_tangent = AssimpUtils::convert_assimp_vec_to_glm_vec3(mesh->mTangents[vert_idx]);
      }

      if (m_params.m_color && mesh->HasVertexColors(0))
      {
        auto color     = AssimpUtils::convert_assimp_color_to_glm_vec4(mesh->mColors[0][vert_idx]);
        vertex.m_color = glm::vec3(color);
      }
      else { vertex.m_color = glm::vec3(1); }

      vertex_buffer.push_back(vertex);
    }

    if (mesh->HasBones()) { process_mesh_bones(vertex_bias, mesh, scene, vertex_buffer); }

    uint32_t number_of_indices = 0;
    for (uint32_t face_idx = 0; face_idx < mesh->mNumFaces; face_idx++)
    {
      aiFace face = mesh->mFaces[face_idx];
      for (uint32_t ind_idx = 0; ind_idx < face.mNumIndices; ind_idx++)
      {
        index_buffer.push_back(vertex_bias + face.mIndices[ind_idx]);
        number_of_indices += 1;
      }
    }

    n_mesh.m_index_count = number_of_indices;
    n_mesh.m_material    = load_material(scene->mMaterials[mesh->mMaterialIndex]);
  }

  void Model::process_node(ModelNode* n_node,
                           aiNode* node,
                           const aiScene* scene,
                           std::vector<uint32_t>& index_buffer,
                           std::vector<Vertex>& vertex_buffer)
  {
    n_node->m_name           = node->mName.data;
    n_node->m_transformation = AssimpUtils::convert_assimp_mat_to_glm_mat4(node->mTransformation);

    for (uint32_t mesh_idx = 0; mesh_idx < node->mNumMeshes; mesh_idx++)
    {
      aiMesh* mesh = scene->mMeshes[node->mMeshes[mesh_idx]];

      n_node->m_meshes.push_back(static_cast<uint32_t>(m_meshes.size()));
      m_meshes.push_back({});

      process_mesh(m_meshes.back(), mesh, scene, index_buffer, vertex_buffer);
    }
    n_node->m_has_meshes = n_node->m_meshes.size() != 0;

    for (uint32_t child_idx = 0; child_idx < node->mNumChildren; child_idx++)
    {
      n_node->m_children.push_back(static_cast<uint32_t>(m_nodes.size()));

      ModelNode* new_node = new ModelNode();
      m_nodes.push_back(new_node);

      process_node(new_node, node->mChildren[child_idx], scene, index_buffer, vertex_buffer);
    }
  }

  Model::Model(const std::string& path_to_model, ModelParams params) : m_bone_counter{ 0 }, m_params{ params }
  {
    m_dir = path_to_model.substr(0, path_to_model.find_last_of('/'));

    Assimp::Importer importer;

    uint32_t assimp_params = m_params.m_load_process_flags;
    if (m_params.m_tangent) { assimp_params = aiProcess_CalcTangentSpace; }

    auto scene = importer.ReadFile((ResourceSystem::get_asset_base_path() / path_to_model).string().c_str(),
                                   aiProcess_Triangulate | assimp_params);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
      ESP_CORE_ERROR("Failed to load model");
      throw std::runtime_error(importer.GetErrorString());
    }

    m_meshes.reserve(scene->mNumMeshes);
    std::vector<uint32_t> index_buffer;
    std::vector<Vertex> vertex_buffer;

    process_node(&m_root_node, scene->mRootNode, scene, index_buffer, vertex_buffer);

    std::vector<uint8_t> vertex_byte_buffer;
    m_params.parse_to_vertex_byte_buffer(vertex_buffer, vertex_byte_buffer);

    m_vertex_buffer = EspVertexBuffer::create(vertex_byte_buffer.data(), sizeof(uint8_t), vertex_byte_buffer.size());
    m_index_buffer  = EspIndexBuffer::create(index_buffer.data(), index_buffer.size());

    for (uint32_t anim_idx = 0; anim_idx < scene->mNumAnimations; ++anim_idx)
    {
      m_animations.push_back(std::make_shared<Animation>(scene->mAnimations[anim_idx], *this));
    }

    set_renderer_flags();
    precompute_transform_matrices(&m_root_node, glm::mat4(1));
  }

  Model::Model(std::vector<Vertex>& vertex_buffer,
               std::vector<uint32_t>& index_buffer,
               const std::vector<std::shared_ptr<EspTexture>>& textures,
               ModelParams params) : m_bone_counter{ 0 }, m_params{ params }
  {
    m_root_node.m_transformation = glm::mat4(1);
    m_root_node.m_meshes.push_back(0);
    auto material = textures.empty() ? nullptr : MaterialSystem::acquire(textures, m_params.m_material_texture_layout);
    m_meshes.push_back(
        { .m_first_index = 0, .m_index_count = static_cast<uint32_t>(index_buffer.size()), .m_material = material });

    std::vector<uint8_t> vertex_byte_buffer;
    m_params.parse_to_vertex_byte_buffer(vertex_buffer, vertex_byte_buffer);

    m_vertex_buffer = EspVertexBuffer::create(vertex_byte_buffer.data(), sizeof(uint8_t), vertex_byte_buffer.size());
    m_index_buffer  = EspIndexBuffer::create(index_buffer.data(), index_buffer.size());

    set_renderer_flags();
    precompute_transform_matrices(&m_root_node, glm::mat4(1));
  }

  void Model::set_vertex_buffer(std::vector<Vertex>& vertex_buffer)
  {
    std::vector<uint8_t> vertex_byte_buffer;
    m_params.parse_to_vertex_byte_buffer(vertex_buffer, vertex_byte_buffer);
    m_vertex_buffer = EspVertexBuffer::create(vertex_byte_buffer.data(), sizeof(uint8_t), vertex_byte_buffer.size());
  }

  void Model::set_index_buffer(std::vector<uint32_t>& index_buffer, uint32_t buffer_index)
  {
    m_index_buffer                       = EspIndexBuffer::create(index_buffer.data(), index_buffer.size());
    m_meshes[buffer_index].m_index_count = index_buffer.size();
  }

  Model::~Model()
  {
    for (auto& node : m_nodes)
    {
      delete node;
    }

    m_nodes.clear();
  }
} // namespace esp
