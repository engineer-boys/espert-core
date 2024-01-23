#include "Model.hh"
#include "Core/Resources/Systems/ResourceSystem.hh"

namespace esp
{
  Model::Builder& Model::Builder::load_model(const std::string& filepath, const ModelParams& params)
  {
    Assimp::Importer importer;
    auto scene = importer.ReadFile((ResourceSystem::get_asset_base_path() / filepath).string().c_str(),
                                   aiProcess_Triangulate | params.p_flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
      ESP_CORE_ERROR("Failed to load model");
      throw std::runtime_error(importer.GetErrorString());
    }

    m_dir = filepath.substr(0, filepath.find_last_of('/'));

    process_node(scene->mRootNode, scene, params);

    return *this;
  }

  void Model::Builder::process_node(aiNode* node, const aiScene* scene, const ModelParams& params)
  {
    for (uint32_t i = 0; i < node->mNumMeshes; i++)
    {
      aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
      process_mesh(mesh, scene, params);
    }

    for (uint32_t i = 0; i < node->mNumChildren; i++)
    {
      process_node(node->mChildren[i], scene, params);
    }
  }

  Model::Builder& Model::Builder::set_shader(std::shared_ptr<EspShader> shader)
  {
    m_shader = shader;
    return *this;
  }

  void Model::Builder::process_mesh(aiMesh* mesh, const aiScene* scene, const ModelParams& params)
  {
    std::vector<Mesh::Vertex> vertices;
    std::vector<uint32_t> indices;
    std::shared_ptr<Material> material;

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

      if ((params.p_flags & aiProcess_CalcTangentSpace) != 0)
      {
        vertex.m_tangent = glm::vec3{ mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
      }

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
    if (params.load_material && mesh->mMaterialIndex >= 0)
    {
      aiMaterial* ai_material = scene->mMaterials[mesh->mMaterialIndex];
      std::vector<std::shared_ptr<EspTexture>> textures;

      for (const auto& layout : params.layouts)
      {
        auto texture = load_material_texture(ai_material, esp_texture_type_to_assimp(layout.type));
        if (texture) textures.push_back(texture);
      }

      material = MaterialSystem::acquire(textures, params.layouts);
      if (!m_material_uniform_managers.contains(material))
      {
        auto uniform_manager = material->create_uniform_manager(m_shader);
        if (uniform_manager) m_material_uniform_managers.insert({ material, std::move(uniform_manager) });
      }
    }

    m_meshes.push_back(std::make_shared<Mesh>(vertices, indices, material));
  }

  std::shared_ptr<EspTexture> Model::Builder::load_material_texture(aiMaterial* mat, aiTextureType type)
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

  Model::Model(esp::Model::Builder& builder) :
      m_meshes{ std::move(builder.m_meshes) },
      m_material_uniform_managers{ std::move(builder.m_material_uniform_managers) }
  {
  }

  Model::Model(std::shared_ptr<Mesh>& mesh) : m_meshes{ std::move(mesh) }, m_material_uniform_managers{} {}

  Model::Model(std::shared_ptr<Mesh>& mesh, std::shared_ptr<EspShader>& shader) : Model(mesh)
  {
    m_material_uniform_managers.insert({ mesh->m_material, mesh->m_material->create_uniform_manager(shader) });
  }

  void Model::draw()
  {
    for (auto& mesh : m_meshes)
    {
      if (mesh->m_material && m_material_uniform_managers.contains(mesh->m_material))
        m_material_uniform_managers.at(mesh->m_material)->attach();
      if (m_has_instance_buffer) { mesh->draw(*m_instance_buffer); }
      else { mesh->draw(); }
    }
  }

  void Model::draw_raw()
  {
    for (auto& mesh : m_meshes)
    {
      mesh->draw_raw();
    }
  }
} // namespace esp