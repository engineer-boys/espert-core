#ifndef RENDER_API_ESP_SHADER_CONFIG_HH
#define RENDER_API_ESP_SHADER_CONFIG_HH

#include "esppch.hh"

#include "Core/RenderAPI/Pipeline/EspAttrFormat.hh"
#include "Core/RenderAPI/Pipeline/Types/EspCompareOp.hh"
#include "Core/RenderAPI/PipelineOrdering/Block/Types/EspBlockFormat.hh"
#include "Core/RenderAPI/PipelineOrdering/Block/Types/EspDepthBlockFormat.hh"
#include "Core/RenderAPI/PipelineOrdering/Block/Types/EspSampleCountFlag.hh"
#include "Core/RenderAPI/Resources/EspSpecializationConstant.hh"
#include "Core/RenderAPI/Uniforms/EspUniformMetaData.hh"

namespace esp
{
  struct EspShaderVertexInputConfig
  {
    uint32_t binding;
    uint32_t location;
    EspVertexInputRate input_rate;
  };

  inline bool operator==(const EspShaderVertexInputConfig& a, const EspShaderVertexInputConfig& b)
  {
    return a.binding == b.binding && a.location == b.location && a.input_rate == b.input_rate;
  }

  struct EspShaderDepthtestConfig
  {
    bool enable;
    EspDepthBlockFormat format;
    EspCompareOp compare_op;
  };

  inline bool operator==(const EspShaderDepthtestConfig& a, const EspShaderDepthtestConfig& b)
  {
    return a.enable == b.enable && a.format == b.format && a.compare_op == b.compare_op;
  }

  struct EspShaderMultisamplingConfig
  {
    bool enable;
    EspSampleCountFlag sample_count_flag;
  };

  inline bool operator==(const EspShaderMultisamplingConfig& a, const EspShaderMultisamplingConfig& b)
  {
    return a.enable == b.enable && a.sample_count_flag == b.sample_count_flag;
  }

  struct EspShaderConfig
  {
    EspShaderDepthtestConfig depthtest_config;
    EspShaderMultisamplingConfig multisampling_config;
    std::vector<EspBlockFormat> color_attachement_formats;
    std::vector<EspShaderVertexInputConfig> vertex_input_config;
    EspSpecializationConstantMap spec_const_map;
  };

  inline bool operator==(const EspShaderConfig& a, const EspShaderConfig& b)
  {
    return a.depthtest_config == b.depthtest_config && a.multisampling_config == b.multisampling_config &&
        a.color_attachement_formats == b.color_attachement_formats && a.vertex_input_config == b.vertex_input_config;
  }
} // namespace esp

template<> struct std::hash<esp::EspShaderVertexInputConfig>
{
  std::size_t operator()(const esp::EspShaderVertexInputConfig& k) const
  {
    std::size_t seed = 0;
    esp::hash_combine(seed, k.binding);
    esp::hash_combine(seed, k.location);
    esp::hash_combine(seed, k.input_rate);
    return seed;
  }
};

template<> struct std::hash<esp::EspShaderDepthtestConfig>
{
  std::size_t operator()(const esp::EspShaderDepthtestConfig& k) const
  {
    std::size_t seed = 0;
    esp::hash_combine(seed, k.enable);
    esp::hash_combine(seed, k.format);
    esp::hash_combine(seed, k.compare_op);
    return seed;
  }
};

template<> struct std::hash<esp::EspShaderMultisamplingConfig>
{
  std::size_t operator()(const esp::EspShaderMultisamplingConfig& k) const
  {
    std::size_t seed = 0;
    esp::hash_combine(seed, k.enable);
    esp::hash_combine(seed, k.sample_count_flag);
    return seed;
  }
};

template<> struct std::hash<std::vector<esp::EspBlockFormat>>
{
  std::size_t operator()(const std::vector<esp::EspBlockFormat>& k) const
  {
    std::size_t seed = 0;
    for (const auto& e : k)
    {
      esp::hash_combine(seed, e);
    }
    return seed;
  }
};

template<> struct std::hash<std::vector<esp::EspShaderVertexInputConfig>>
{
  std::size_t operator()(const std::vector<esp::EspShaderVertexInputConfig>& k) const
  {
    std::size_t seed = 0;
    for (const auto& e : k)
    {
      esp::hash_combine(seed, e);
    }
    return seed;
  }
};

template<> struct std::hash<esp::EspShaderConfig>
{
  std::size_t operator()(const esp::EspShaderConfig& k) const
  {
    std::size_t seed = 0;
    esp::hash_combine(seed, k.depthtest_config);
    esp::hash_combine(seed, k.multisampling_config);
    esp::hash_combine(seed, k.color_attachement_formats);
    esp::hash_combine(seed, k.vertex_input_config);
    esp::hash_combine(seed, k.spec_const_map);
    return seed;
  }
};

template<> struct std::hash<std::pair<std::string, esp::EspShaderConfig>>
{
  std::size_t operator()(const std::pair<std::string, esp::EspShaderConfig>& k) const
  {
    std::size_t seed = 0;
    esp::hash_combine(seed, k.first);
    esp::hash_combine(seed, k.second);
    return seed;
  }
};

#endif // RENDER_API_ESP_SHADER_CONFIG_HH