#ifndef VULKAN_RENDER_API_ESP_PUSH_CONSTANT_DATA_H_
#define VULKAN_RENDER_API_ESP_PUSH_CONSTANT_DATA_H_

namespace esp
{
  // TODO: try to create generic class for push constant data
  struct EspPushConstantData
  {
    template<typename T> inline static VkPushConstantRange create_range()
    {
      VkPushConstantRange push_constant_range{};
      push_constant_range.stageFlags =
          VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
      push_constant_range.offset = 0;
      push_constant_range.size   = sizeof(T);

      return push_constant_range;
    }

    template<typename T>
    inline void bind(VkCommandBuffer command_buffer, VkPipelineLayout layout)
    {
      vkCmdPushConstants(command_buffer,
                         layout,
                         VK_SHADER_STAGE_VERTEX_BIT |
                             VK_SHADER_STAGE_FRAGMENT_BIT,
                         0,
                         sizeof(T),
                         this);
    }
  };
} // namespace esp

#endif // VULKAN_RENDER_API_ESP_PUSH_CONSTANT_DATA_H_
