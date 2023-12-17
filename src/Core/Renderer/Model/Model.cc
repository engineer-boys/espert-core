#include "Model.hh"
#include "Core/Systems/ResourceSystem.hh"

namespace esp
{
  Model::Builder& Model::Builder::load_model(const std::string& filepath, unsigned int p_flags)
  {
    Assimp::Importer importer;
    auto scene = importer.ReadFile(ResourceSystem::get_asset_base_path() / filepath, aiProcess_Triangulate | p_flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
      ESP_CORE_ERROR("Failed to load model");
      throw std::runtime_error(importer.GetErrorString());
    }

    m_dir = filepath.substr(0, filepath.find_last_of('/'));

    process_node(scene->mRootNode, scene);

    return *this;
  }

  void Model::Builder::process_node(aiNode* node, const aiScene* scene)
  {
    for (uint32_t i = 0; i < node->mNumMeshes; i++)
    {
      aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
      process_mesh(mesh, scene);
    }

    for (uint32_t i = 0; i < node->mNumChildren; i++)
    {
      process_node(node->mChildren[i], scene);
    }
  }

  void Model::Builder::process_mesh(aiMesh* mesh, const aiScene* scene)
  {
    std::vector<Mesh::Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<std::shared_ptr<EspTexture>> textures;

    // process vertices
    for (uint32_t i = 0; i < mesh->mNumVertices; i++)
    {
      Mesh::Vertex vertex{};

      vertex.m_position = glm::vec3{ mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
      vertex.m_normal   = glm::vec3{ mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
      if (mesh->mTextureCoords[0])
      {
        vertex.m_tex_coord = glm::vec2{ mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
      }
      else { vertex.m_tex_coord = glm::vec2{ 0.f, 0.f }; }

      vertices.push_back(vertex);
    }

    // process indices
    for (uint32_t i = 0; i < mesh->mNumFaces; i++)
    {
      aiFace face = mesh->mFaces[i];
      for (uint32_t j = 0; j < face.mNumIndices; j++)
      {
        indices.push_back(face.mIndices[j]);
      }
    }

    // process material
    if (mesh->mMaterialIndex >= 0)
    {
      aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

      auto diffuse_maps = load_material_textures(material, aiTextureType_DIFFUSE);
      textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
      auto specular_maps = load_material_textures(material, aiTextureType_SPECULAR);
      textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
    }

    m_meshes.push_back(std::make_shared<Mesh>(vertices, indices, textures));
  }

  std::vector<std::shared_ptr<EspTexture>> Model::Builder::load_material_textures(aiMaterial* mat, aiTextureType type)
  {
    std::vector<std::shared_ptr<EspTexture>> textures;

    for (uint32_t i = 0; i < mat->GetTextureCount(type); i++)
    {
      aiString str;
      mat->GetTexture(type, i, &str);

      textures.push_back(TextureSystem::acquire(fs::path(m_dir) / str.C_Str()));
    }

    return textures;
  }

  Model::Model(esp::Model::Builder& builder, EspPipeline& pipeline) : m_meshes{ std::move(builder.m_meshes) }
  {
    add_materials(pipeline);
  }

  Model::Model(std::shared_ptr<Mesh> mesh, std::vector<std::shared_ptr<EspTexture>> textures, EspPipeline& pipeline) :
      m_meshes{ std::move(mesh) }
  {
    m_meshes[0]->m_textures = textures;
    if (!textures.empty()) { add_materials(pipeline); }
  }

  void Model::draw()
  {
    for (auto& mesh : m_meshes)
    {
      if (m_has_instance_buffer) { mesh->draw(*m_instance_buffer); }
      else { mesh->draw(); }
    }
  }

  void Model::add_materials(esp::EspPipeline& pipeline)
  {
    for (auto& mesh : m_meshes)
    {
      mesh->add_material(pipeline);
    }
  }
} // namespace esp