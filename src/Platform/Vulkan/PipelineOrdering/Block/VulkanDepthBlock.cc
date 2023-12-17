#include "VulkanDepthBlock.hh"

namespace esp
{
  VulkanDepthBlock::VulkanDepthBlock(uint32_t width, uint32_t height, glm::vec3 clear_color) :
      EspDepthBlock(width, height, clear_color)
  {
  }

  VulkanDepthBlock::~VulkanDepthBlock() {}
} // namespace esp
