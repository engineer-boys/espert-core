#include "Core/RenderAPI/Resources/SpirvReflection.hh"

namespace esp
{
  template<ShaderPropertyType T>
  inline void read_shader_property(const spirv_cross::Compiler& compiler,
                                   VkShaderStageFlagBits stage,
                                   std::vector<ShaderProperty>& properties)
  {
    ESP_CORE_ERROR("Not implemented! Read shader properties of type.");
  }

  template<spv::Decoration T>
  inline void read_property_decoration(const spirv_cross::Compiler& compiler,
                                       const spirv_cross::Resource& resource,
                                       ShaderProperty& shader_property)
  {
    ESP_CORE_ERROR("Not implemented! Read property decoration of type.");
  }

  template<>
  inline void read_property_decoration<spv::DecorationLocation>(const spirv_cross::Compiler& compiler,
                                                                const spirv_cross::Resource& resource,
                                                                ShaderProperty& shader_property)
  {
    shader_property.location = compiler.get_decoration(resource.id, spv::DecorationLocation);
  }

  template<>
  inline void read_property_decoration<spv::DecorationDescriptorSet>(const spirv_cross::Compiler& compiler,
                                                                     const spirv_cross::Resource& resource,
                                                                     ShaderProperty& shader_property)
  {
    shader_property.set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
  }

  template<>
  inline void read_property_decoration<spv::DecorationBinding>(const spirv_cross::Compiler& compiler,
                                                               const spirv_cross::Resource& resource,
                                                               ShaderProperty& shader_property)
  {
    shader_property.binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
  }

  template<>
  inline void read_property_decoration<spv::DecorationInputAttachmentIndex>(const spirv_cross::Compiler& compiler,
                                                                            const spirv_cross::Resource& resource,
                                                                            ShaderProperty& shader_property)
  {
    shader_property.input_attachment_index = compiler.get_decoration(resource.id, spv::DecorationInputAttachmentIndex);
  }

  template<>
  inline void read_property_decoration<spv::DecorationNonWritable>(const spirv_cross::Compiler& compiler,
                                                                   const spirv_cross::Resource& resource,
                                                                   ShaderProperty& shader_property)
  {
    shader_property.qualifiers |= ShaderPropertyQualifiers::NonWritable;
  }

  template<>
  inline void read_property_decoration<spv::DecorationNonReadable>(const spirv_cross::Compiler& compiler,
                                                                   const spirv_cross::Resource& resource,
                                                                   ShaderProperty& shader_property)
  {
    shader_property.qualifiers |= ShaderPropertyQualifiers::NonReadable;
  }

  inline void read_property_vec_size(const spirv_cross::Compiler& compiler,
                                     const spirv_cross::Resource& resource,
                                     ShaderProperty& shader_property)
  {
    const auto& spirv_type = compiler.get_type_from_variable(resource.id);

    shader_property.vec_size = spirv_type.vecsize;
    shader_property.columns  = spirv_type.columns;
  }

  inline void read_property_array_size(const spirv_cross::Compiler& compiler,
                                       const spirv_cross::Resource& resource,
                                       ShaderProperty& shader_property)
  {
    const auto& spirv_type = compiler.get_type_from_variable(resource.id);

    shader_property.array_size = spirv_type.array.size() ? spirv_type.array[0] : 1;
  }

  inline void read_property_size(const spirv_cross::Compiler& compiler,
                                 const spirv_cross::Resource& resource,
                                 ShaderProperty& shader_property)
  {
    const auto& spirv_type = compiler.get_type_from_variable(resource.id);

    shader_property.size = static_cast<uint32_t>(compiler.get_declared_struct_size(spirv_type));
  }

  inline void read_property_size(const spirv_cross::Compiler& compiler,
                                 const spirv_cross::SPIRConstant& constant,
                                 ShaderProperty& shader_property)
  {
    auto spirv_type = compiler.get_type(constant.constant_type);

    switch (spirv_type.basetype)
    {
    case spirv_cross::SPIRType::BaseType::Boolean:
    case spirv_cross::SPIRType::BaseType::Char:
    case spirv_cross::SPIRType::BaseType::Int:
    case spirv_cross::SPIRType::BaseType::UInt:
    case spirv_cross::SPIRType::BaseType::Float:
      shader_property.size = 4;
      break;
    case spirv_cross::SPIRType::BaseType::Int64:
    case spirv_cross::SPIRType::BaseType::UInt64:
    case spirv_cross::SPIRType::BaseType::Double:
      shader_property.size = 8;
      break;
    default:
      shader_property.size = 0;
      break;
    }
  }

  template<>
  inline void read_shader_property<ShaderPropertyType::Input>(const spirv_cross::Compiler& compiler,
                                                              VkShaderStageFlagBits stage,
                                                              std::vector<ShaderProperty>& properties)
  {
    auto input_properties = compiler.get_shader_resources().stage_inputs;

    for (auto& property : input_properties)
    {
      ShaderProperty shader_property{};
      shader_property.type   = ShaderPropertyType::Input;
      shader_property.stages = stage;
      shader_property.name   = property.name;

      read_property_vec_size(compiler, property, shader_property);
      read_property_array_size(compiler, property, shader_property);
      read_property_decoration<spv::DecorationLocation>(compiler, property, shader_property);

      properties.push_back(shader_property);
    }
  }

  template<>
  inline void read_shader_property<ShaderPropertyType::InputAttachment>(const spirv_cross::Compiler& compiler,
                                                                        VkShaderStageFlagBits stage,
                                                                        std::vector<ShaderProperty>& properties)
  {
    auto subpass_resources = compiler.get_shader_resources().subpass_inputs;

    for (auto& resource : subpass_resources)
    {
      ShaderProperty shader_property{};
      shader_property.type   = ShaderPropertyType::InputAttachment;
      shader_property.stages = VK_SHADER_STAGE_FRAGMENT_BIT;
      shader_property.name   = resource.name;

      read_property_array_size(compiler, resource, shader_property);
      read_property_decoration<spv::DecorationInputAttachmentIndex>(compiler, resource, shader_property);
      read_property_decoration<spv::DecorationDescriptorSet>(compiler, resource, shader_property);
      read_property_decoration<spv::DecorationBinding>(compiler, resource, shader_property);

      properties.push_back(shader_property);
    }
  }

  template<>
  inline void read_shader_property<ShaderPropertyType::Output>(const spirv_cross::Compiler& compiler,
                                                               VkShaderStageFlagBits stage,
                                                               std::vector<ShaderProperty>& properties)
  {
    auto output_resources = compiler.get_shader_resources().stage_outputs;

    for (auto& resource : output_resources)
    {
      ShaderProperty shader_property{};
      shader_property.type   = ShaderPropertyType::Output;
      shader_property.stages = stage;
      shader_property.name   = resource.name;

      read_property_array_size(compiler, resource, shader_property);
      read_property_vec_size(compiler, resource, shader_property);
      read_property_decoration<spv::DecorationLocation>(compiler, resource, shader_property);

      properties.push_back(shader_property);
    }
  }

  template<>
  inline void read_shader_property<ShaderPropertyType::Image>(const spirv_cross::Compiler& compiler,
                                                              VkShaderStageFlagBits stage,
                                                              std::vector<ShaderProperty>& properties)
  {
    auto image_resources = compiler.get_shader_resources().separate_images;

    for (auto& resource : image_resources)
    {
      ShaderProperty shader_property{};
      shader_property.type   = ShaderPropertyType::Image;
      shader_property.stages = stage;
      shader_property.name   = resource.name;

      read_property_array_size(compiler, resource, shader_property);
      read_property_decoration<spv::DecorationDescriptorSet>(compiler, resource, shader_property);
      read_property_decoration<spv::DecorationBinding>(compiler, resource, shader_property);

      properties.push_back(shader_property);
    }
  }

  template<>
  inline void read_shader_property<ShaderPropertyType::ImageSampler>(const spirv_cross::Compiler& compiler,
                                                                     VkShaderStageFlagBits stage,
                                                                     std::vector<ShaderProperty>& properties)
  {
    auto image_resources = compiler.get_shader_resources().sampled_images;

    for (auto& resource : image_resources)
    {
      ShaderProperty shader_property{};
      shader_property.type   = ShaderPropertyType::ImageSampler;
      shader_property.stages = stage;
      shader_property.name   = resource.name;

      read_property_array_size(compiler, resource, shader_property);
      read_property_decoration<spv::DecorationDescriptorSet>(compiler, resource, shader_property);
      read_property_decoration<spv::DecorationBinding>(compiler, resource, shader_property);

      properties.push_back(shader_property);
    }
  }

  template<>
  inline void read_shader_property<ShaderPropertyType::ImageStorage>(const spirv_cross::Compiler& compiler,
                                                                     VkShaderStageFlagBits stage,
                                                                     std::vector<ShaderProperty>& properties)
  {
    auto storage_resources = compiler.get_shader_resources().storage_images;

    for (auto& resource : storage_resources)
    {
      ShaderProperty shader_property{};
      shader_property.type   = ShaderPropertyType::ImageStorage;
      shader_property.stages = stage;
      shader_property.name   = resource.name;

      read_property_array_size(compiler, resource, shader_property);
      read_property_decoration<spv::DecorationNonReadable>(compiler, resource, shader_property);
      read_property_decoration<spv::DecorationNonWritable>(compiler, resource, shader_property);
      read_property_decoration<spv::DecorationDescriptorSet>(compiler, resource, shader_property);
      read_property_decoration<spv::DecorationBinding>(compiler, resource, shader_property);

      properties.push_back(shader_property);
    }
  }

  template<>
  inline void read_shader_property<ShaderPropertyType::Sampler>(const spirv_cross::Compiler& compiler,
                                                                VkShaderStageFlagBits stage,
                                                                std::vector<ShaderProperty>& properties)
  {
    auto sampler_resources = compiler.get_shader_resources().separate_samplers;

    for (auto& resource : sampler_resources)
    {
      ShaderProperty shader_property{};
      shader_property.type   = ShaderPropertyType::Sampler;
      shader_property.stages = stage;
      shader_property.name   = resource.name;

      read_property_array_size(compiler, resource, shader_property);
      read_property_decoration<spv::DecorationDescriptorSet>(compiler, resource, shader_property);
      read_property_decoration<spv::DecorationBinding>(compiler, resource, shader_property);

      properties.push_back(shader_property);
    }
  }

  template<>
  inline void read_shader_property<ShaderPropertyType::BufferUniform>(const spirv_cross::Compiler& compiler,
                                                                      VkShaderStageFlagBits stage,
                                                                      std::vector<ShaderProperty>& properties)
  {
    auto uniform_resources = compiler.get_shader_resources().uniform_buffers;

    for (auto& resource : uniform_resources)
    {
      ShaderProperty shader_property{};
      shader_property.type   = ShaderPropertyType::BufferUniform;
      shader_property.stages = stage;
      shader_property.name   = resource.name;

      read_property_size(compiler, resource, shader_property);
      read_property_array_size(compiler, resource, shader_property);
      read_property_decoration<spv::DecorationDescriptorSet>(compiler, resource, shader_property);
      read_property_decoration<spv::DecorationBinding>(compiler, resource, shader_property);

      properties.push_back(shader_property);
    }
  }

  template<>
  inline void read_shader_property<ShaderPropertyType::BufferStorage>(const spirv_cross::Compiler& compiler,
                                                                      VkShaderStageFlagBits stage,
                                                                      std::vector<ShaderProperty>& properties)
  {
    auto storage_resources = compiler.get_shader_resources().storage_buffers;

    for (auto& resource : storage_resources)
    {
      ShaderProperty shader_property;
      shader_property.type   = ShaderPropertyType::BufferStorage;
      shader_property.stages = stage;
      shader_property.name   = resource.name;

      read_property_size(compiler, resource, shader_property);
      read_property_array_size(compiler, resource, shader_property);
      read_property_decoration<spv::DecorationNonReadable>(compiler, resource, shader_property);
      read_property_decoration<spv::DecorationNonWritable>(compiler, resource, shader_property);
      read_property_decoration<spv::DecorationDescriptorSet>(compiler, resource, shader_property);
      read_property_decoration<spv::DecorationBinding>(compiler, resource, shader_property);

      properties.push_back(shader_property);
    }
  }

  void SpirvReflection::parse_shader_properties(const spirv_cross::Compiler& compiler,
                                                VkShaderStageFlagBits stage,
                                                std::vector<ShaderProperty>& properties)
  {
    read_shader_property<ShaderPropertyType::Input>(compiler, stage, properties);
    read_shader_property<ShaderPropertyType::InputAttachment>(compiler, stage, properties);
    read_shader_property<ShaderPropertyType::Output>(compiler, stage, properties);
    read_shader_property<ShaderPropertyType::Image>(compiler, stage, properties);
    read_shader_property<ShaderPropertyType::ImageSampler>(compiler, stage, properties);
    read_shader_property<ShaderPropertyType::ImageStorage>(compiler, stage, properties);
    read_shader_property<ShaderPropertyType::Sampler>(compiler, stage, properties);
    read_shader_property<ShaderPropertyType::BufferUniform>(compiler, stage, properties);
    read_shader_property<ShaderPropertyType::BufferStorage>(compiler, stage, properties);
  }

  void SpirvReflection::parse_push_constants(const spirv_cross::Compiler& compiler,
                                             VkShaderStageFlagBits stage,
                                             std::vector<ShaderProperty>& properties)
  {
    auto shader_properties = compiler.get_shader_resources();

    for (auto& property : shader_properties.push_constant_buffers)
    {
      const auto& spivr_type = compiler.get_type_from_variable(property.id);

      std::uint32_t offset = std::numeric_limits<std::uint32_t>::max();

      for (auto i = 0U; i < spivr_type.member_types.size(); ++i)
      {
        auto mem_offset = compiler.get_member_decoration(spivr_type.self, i, spv::DecorationOffset);

        offset = std::min(offset, mem_offset);
      }

      ShaderProperty shader_property{};
      shader_property.type   = ShaderPropertyType::PushConstant;
      shader_property.stages = stage;
      shader_property.name   = property.name;
      shader_property.offset = offset;

      read_property_size(compiler, property, shader_property);

      shader_property.size -= shader_property.offset;

      properties.push_back(shader_property);
    }
  }

} // namespace esp
