#ifndef ESPERT_CORE_RENDERAPI_RESOURCES_SHADERPROPERTY_HH
#define ESPERT_CORE_RENDERAPI_RESOURCES_SHADERPROPERTY_HH

#include "esppch.hh"

namespace esp
{
  using ShaderStageFlags = uint8_t;

  enum class ShaderStage : uint8_t
  {
    ALL                    = 0x00,
    VERTEX                 = 0x01,
    TESSELATION_CONTROL    = 0x02,
    TESSELATION_EVALUATION = 0x04,
    GEOMETRY               = 0x08,
    FRAGMENT               = 0x10,
    COMPUTE                = 0x20
  };

  enum class ShaderPropertyType
  {
    Input,
    InputAttachment,
    Output,
    Image,
    ImageSampler,
    ImageStorage,
    Sampler,
    BufferUniform,
    BufferStorage,
    PushConstant,
    SpecializationConstant,
    All
  };

  enum class ShaderPropertyMode
  {
    Static,
    Dynamic,
    UpdateAfterBind
  };

  struct ShaderPropertyQualifiers
  {
    enum : uint32_t
    {
      None        = 0,
      NonReadable = 1,
      NonWritable = 2,
    };
  };

  struct ShaderProperty
  {
    VkShaderStageFlags stages;

    ShaderPropertyType type;

    ShaderPropertyMode mode;

    uint32_t set;

    uint32_t binding;

    uint32_t location;

    uint32_t input_attachment_index;

    uint32_t vec_size;

    uint32_t columns;

    uint32_t array_size;

    uint32_t offset;

    uint32_t size;

    uint32_t constant_id;

    uint32_t qualifiers;

    std::string name;
  };
} // namespace esp

#endif // ESPERT_CORE_RENDERAPI_RESOURCES_SHADERPROPERTY_HH