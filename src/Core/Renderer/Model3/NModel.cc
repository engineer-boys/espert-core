#include "NModel.hh"

#include "Core/Renderer/Utils/AssimpUtils.hh"
#include "Core/Resources/Systems/ResourceSystem.hh"

#include "Core/RenderAPI/Work/EspJob.hh"

#include "Core/Renderer/Model3/Animation/Animation.hh"

#include "Core/Resources/Systems/TextureSystem.hh"

#include <map>

#define MAX_BONE_WEIGHTS 4

namespace esp
{
  void NModel::load_materials(std::string path_to_model, const aiScene* scene)
  {
    std::map<std::string, uint32_t> texture_name_to_idx;
    auto path = fs::path(path_to_model);

    for (uint32_t mtr_idx = 0; mtr_idx < scene->mNumMaterials; ++mtr_idx)
    {
      NMaterial n_material = {};
      auto material        = scene->mMaterials[mtr_idx];

      // albedo
      if (m_params.m_material_params.m_albedo && material->GetTextureCount(aiTextureType_DIFFUSE) != 0)
      {
        aiString ai_name;
        material->GetTexture(aiTextureType_DIFFUSE, 0, &ai_name);

        std::string txt_name = ai_name.C_Str();
        if (texture_name_to_idx.contains(txt_name) == 0)
        {
          texture_name_to_idx[txt_name] = m_textures.size();
          n_material.m_albedo           = m_textures.size();

          m_textures.push_back(TextureSystem::acquire(path.parent_path() / txt_name));
        }
        else { n_material.m_albedo = texture_name_to_idx[txt_name]; }
      }

      // normal
      if (m_params.m_material_params.m_normal && material->GetTextureCount(aiTextureType_NORMALS) != 0)
      {
        aiString ai_name;
        material->GetTexture(aiTextureType_NORMALS, 0, &ai_name);

        std::string txt_name = ai_name.C_Str();
        if (texture_name_to_idx.contains(txt_name) == 0)
        {
          texture_name_to_idx[txt_name] = m_textures.size();
          n_material.m_normal           = m_textures.size();

          m_textures.push_back(TextureSystem::acquire(path.parent_path() / txt_name));
        }
        else { n_material.m_normal = texture_name_to_idx[txt_name]; }
      }

      // metallic
      if (m_params.m_material_params.m_metallic && material->GetTextureCount(aiTextureType_METALNESS) != 0)
      {
        aiString ai_name;
        material->GetTexture(aiTextureType_METALNESS, 0, &ai_name);

        std::string txt_name = ai_name.C_Str();
        if (texture_name_to_idx.contains(txt_name) == 0)
        {
          texture_name_to_idx[txt_name] = m_textures.size();
          n_material.m_metallic         = m_textures.size();

          m_textures.push_back(TextureSystem::acquire(path.parent_path() / txt_name));
        }
        else { n_material.m_metallic = texture_name_to_idx[txt_name]; }
      }

      // roughness
      if (m_params.m_material_params.m_roughness && material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) != 0)
      {
        aiString ai_name;
        material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &ai_name);

        std::string txt_name = ai_name.C_Str();
        if (texture_name_to_idx.contains(txt_name) == 0)
        {
          texture_name_to_idx[txt_name] = m_textures.size();
          n_material.m_roughness        = m_textures.size();

          m_textures.push_back(TextureSystem::acquire(path.parent_path() / txt_name));
        }
        else { n_material.m_roughness = texture_name_to_idx[txt_name]; }
      }

      // ao
      if (m_params.m_material_params.m_ao && material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION) != 0)
      {
        aiString ai_name;
        material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &ai_name);

        std::string txt_name = ai_name.C_Str();
        if (texture_name_to_idx.contains(txt_name) == 0)
        {
          texture_name_to_idx[txt_name] = m_textures.size();
          n_material.m_ao               = m_textures.size();

          m_textures.push_back(TextureSystem::acquire(path.parent_path() / txt_name));
        }
        else { n_material.m_ao = texture_name_to_idx[txt_name]; }
      }

      if (n_material.is_empty()) { continue; }

      m_materials.push_back(n_material);
    }
  }

  void NModel::precompute_transform_matrices(NNode* node, glm::mat4 prev_matrix)
  {
    node->m_precomputed_transformation = prev_matrix * node->m_transformation;

    for (auto child : node->m_children)
    {
      precompute_transform_matrices(m_nodes[child], node->m_precomputed_transformation);
    }
  }

  void NModel::process_mesh_bones(uint32_t vertex_bias,
                                  const aiMesh* mesh,
                                  const aiScene* scene,
                                  std::vector<NVertex>& vertex_buffer)
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

        NVertex& vertex = vertex_buffer[vertex_bias + vertex_id];

        if (vertex_weights[vertex_id] < 4)
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

  void NModel::process_mesh(NMesh& n_mesh,
                            const aiMesh* mesh,
                            const aiScene* scene,
                            std::vector<uint32_t>& index_buffer,
                            std::vector<NVertex>& vertex_buffer)
  {
    uint32_t vertex_bias = static_cast<uint32_t>(vertex_buffer.size());

    n_mesh.m_first_index    = static_cast<uint32_t>(index_buffer.size());
    n_mesh.m_material_index = mesh->mMaterialIndex;

    for (uint32_t vert_idx = 0; vert_idx < mesh->mNumVertices; vert_idx++)
    {
      NVertex vertex = {};

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
  }

  void NModel::process_node(NNode* n_node,
                            aiNode* node,
                            const aiScene* scene,
                            std::vector<uint32_t>& index_buffer,
                            std::vector<NVertex>& vertex_buffer)
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
    n_node->m_mesh_node = n_node->m_meshes.size() != 0;

    for (uint32_t child_idx = 0; child_idx < node->mNumChildren; child_idx++)
    {
      n_node->m_children.push_back(static_cast<uint32_t>(m_nodes.size()));

      NNode* new_node = new NNode();
      m_nodes.push_back(new_node);

      process_node(new_node, node->mChildren[child_idx], scene, index_buffer, vertex_buffer);
    }
  }

  NModel::NModel(std::string path_to_model, NModelParams params) : m_bone_counter{ 0 }, m_params{ params }
  {
    Assimp::Importer importer;

    uint32_t assimp_params = 0;
    if (m_params.m_tangent) { assimp_params = aiProcess_CalcTangentSpace; }

    auto scene =
        importer.ReadFile(ResourceSystem::get_asset_base_path() / path_to_model, aiProcess_Triangulate | assimp_params);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
      ESP_CORE_ERROR("Failed to load model");
      throw std::runtime_error(importer.GetErrorString());
    }

    load_materials(path_to_model, scene);

    m_meshes.reserve(scene->mNumMeshes);
    std::vector<uint32_t> index_buffer;
    std::vector<NVertex> vertex_buffer;

    process_node(&m_root_node, scene->mRootNode, scene, index_buffer, vertex_buffer);

    std::vector<uint8_t> vertex_byte_buffer;
    m_params.parse_to_vertex_byte_buffer(vertex_buffer, vertex_byte_buffer);

    m_vertex_buffer = EspVertexBuffer::create(vertex_byte_buffer.data(), sizeof(uint8_t), vertex_byte_buffer.size());
    m_index_buffer  = EspIndexBuffer::create(index_buffer.data(), index_buffer.size());

    for (uint32_t anim_idx = 0; anim_idx < scene->mNumAnimations; ++anim_idx)
    {
      m_animations.push_back(std::make_shared<Animation>(scene->mAnimations[anim_idx], *this));
    }

    precompute_transform_matrices(&m_root_node, glm::mat4(1));
  }

  NModel::~NModel()
  {
    for (auto& node : m_nodes)
    {
      delete node;
    }

    m_nodes.clear();
  }

  void NModel::set_localisation(glm::mat4 localisation) { precompute_transform_matrices(&m_root_node, localisation); }
} // namespace esp
