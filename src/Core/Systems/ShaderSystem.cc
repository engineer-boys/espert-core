#include "Core/Systems/ShaderSystem.hh"

#include "Core/RenderAPI/Pipeline/EspPipelineBuilder.hh"
#include "Core/RenderAPI/Resources/SpirvReflection.hh"
#include "Core/RenderAPI/Uniforms/EspUniformMetaData.hh"
#include "Core/Systems/ResourceSystem.hh"

namespace esp
{
  Shader::Shader(std::unique_ptr<ShaderResource> shader_resource)
  {
    auto builder = EspPipelineBuilder::create();

    auto uniform_meta_data = EspUniformMetaData::create();

    shader_resource->enumerate_sources(
        [&builder, &uniform_meta_data](ShaderStage stage, std::shared_ptr<ShaderSource> shader_spv)
        {
          builder->set_shader(stage, shader_spv);
          spirv_cross::Compiler reflection_object(*shader_spv);
          std::vector<ShaderProperty> properties;
          SpirvReflection::parse_shader_properties(reflection_object, stage, properties);
          m_shader_properties[stage] = std::move(properties);
        });

    uniform_meta_data->establish_descriptor_set(m_shader_properties);
    auto vertex_input_resources = pipeline_layout.get_resources(ShaderResourceType::Input, VK_SHADER_STAGE_VERTEX_BIT);

    VertexInputState vertex_input_state;

    for (auto& input_resource : vertex_input_resources)
    {
      sg::VertexAttribute attribute;

      if (!sub_mesh.get_attribute(input_resource.name, attribute)) { continue; }

      VkVertexInputAttributeDescription vertex_attribute{};
      vertex_attribute.binding  = input_resource.location;
      vertex_attribute.format   = attribute.format;
      vertex_attribute.location = input_resource.location;
      vertex_attribute.offset   = attribute.offset;

      vertex_input_state.attributes.push_back(vertex_attribute);

      VkVertexInputBindingDescription vertex_binding{};
      vertex_binding.binding = input_resource.location;
      vertex_binding.stride  = attribute.stride;

      vertex_input_state.bindings.push_back(vertex_binding);
    }

    command_buffer.set_vertex_input_state(vertex_input_state);
    builder->set_pipeline_layout(std::move(uniform_meta_data));

    m_pipeline = builder->build_pipeline();
  }

  void Shader::attach() { m_pipeline->attach(); }

  std::unique_ptr<EspUniformManager> Shader::create_uniform_manager() { return m_pipeline->create_uniform_manager(); }

  ShaderSystem* ShaderSystem::s_instance = nullptr;

  ShaderSystem::ShaderSystem()
  {
    if (ShaderSystem::s_instance != nullptr) { throw std::runtime_error("The shader system instance already exists!"); }

    ShaderSystem::s_instance = this;
  }

  ShaderSystem::~ShaderSystem()
  {
    ShaderSystem::s_instance = nullptr;
    ESP_CORE_TRACE("Shader system shutdown.");
  }

  std::unique_ptr<ShaderSystem> ShaderSystem::create()
  {
    auto shader_system = std::unique_ptr<ShaderSystem>(new ShaderSystem());

    ESP_CORE_TRACE("Shader system initialized.");

    return shader_system;
  }

  std::shared_ptr<Shader> ShaderSystem::acquire(const std::string& name)
  {
    if (s_instance->m_shader_map.contains(name)) return s_instance->m_shader_map.at(name);
    return load(name);
  }

  std::shared_ptr<Shader> ShaderSystem::load(const std::string& name)
  {
    ShaderResourceParams params;
    auto resource        = ResourceSystem::load<ShaderResource>(name, params);
    auto shader_resource = unique_cast<ShaderResource, Resource>(std::move(resource));
    auto shader          = std::make_shared<Shader>(name, std::move(shader_resource));
    s_instance->m_shader_map.insert({ name, shader });
    ESP_CORE_TRACE("Loaded shader {}.", name);
    return shader;
  }
} // namespace esp