#ifndef CORE_RENDER_API_ESP_IMAGE_USAGE_FLAG_HH
#define CORE_RENDER_API_ESP_IMAGE_USAGE_FLAG_HH

#include "esppch.hh"

namespace esp
{
  /*
  Values for this enum  come from Vulkan
  TODO: OpenGL version
  */

  enum class EspImageUsageFlag
  {
    ESP_IMAGE_USAGE_TRANSFER_SRC_BIT                         = 0x00000001,
    ESP_IMAGE_USAGE_TRANSFER_DST_BIT                         = 0x00000002,
    ESP_IMAGE_USAGE_SAMPLED_BIT                              = 0x00000004,
    ESP_IMAGE_USAGE_STORAGE_BIT                              = 0x00000008,
    ESP_IMAGE_USAGE_COLOR_ATTACHMENT_BIT                     = 0x00000010,
    ESP_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT             = 0x00000020,
    ESP_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT                 = 0x00000040,
    ESP_IMAGE_USAGE_INPUT_ATTACHMENT_BIT                     = 0x00000080,
    ESP_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR                 = 0x00000400,
    ESP_IMAGE_USAGE_VIDEO_DECODE_SRC_BIT_KHR                 = 0x00000800,
    ESP_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR                 = 0x00001000,
    ESP_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT             = 0x00000200,
    ESP_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR = 0x00000100,
    ESP_IMAGE_USAGE_HOST_TRANSFER_BIT_EXT                    = 0x00400000,
#ifdef VK_ENABLE_BETA_EXTENSIONS
    ESP_IMAGE_USAGE_VIDEO_ENCODE_DST_BIT_KHR = 0x00002000,
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
    ESP_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR = 0x00004000,
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
    ESP_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR = 0x00008000,
#endif
    ESP_IMAGE_USAGE_ATTACHMENT_FEEDBACK_LOOP_BIT_EXT = 0x00080000,
    ESP_IMAGE_USAGE_INVOCATION_MASK_BIT_HUAWEI       = 0x00040000,
    ESP_IMAGE_USAGE_SAMPLE_WEIGHT_BIT_QCOM           = 0x00100000,
    ESP_IMAGE_USAGE_SAMPLE_BLOCK_MATCH_BIT_QCOM      = 0x00200000,
    ESP_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV        = ESP_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR,
    ESP_IMAGE_USAGE_FLAG_BITS_MAX_ENUM               = 0x7FFFFFFF
  };

  inline EspImageUsageFlag operator|(EspImageUsageFlag lhs, EspImageUsageFlag rhs)
  {
    return static_cast<EspImageUsageFlag>(static_cast<int>(lhs) | static_cast<int>(rhs));
  }

  inline EspImageUsageFlag operator&(EspImageUsageFlag lhs, EspImageUsageFlag rhs)
  {
    return static_cast<EspImageUsageFlag>(static_cast<int>(lhs) & static_cast<int>(rhs));
  }
} // namespace esp

#endif /* CORE_RENDER_API_ESP_IMAGE_USAGE_FLAG_HH */
