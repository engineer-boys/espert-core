#include "VulkanJobs.hh"
#include "Platform/Vulkan/VulkanDevice.hh"
#include "VulkanWorkOrchestrator.hh"

namespace esp
{
  VulkanJobs* VulkanJobs::s_instance = nullptr;

  std::unique_ptr<VulkanJobs> VulkanJobs::create()
  {
    ESP_ASSERT(VulkanJobs::s_instance == nullptr, "VulkanJobs already exists!");
    VulkanJobs::s_instance = new VulkanJobs();
    VulkanJobs::s_instance->init();

    return std::unique_ptr<VulkanJobs>{ VulkanJobs::s_instance };
  }

  VulkanJobs::VulkanJobs() {}

  VulkanJobs::~VulkanJobs() {}

  void VulkanJobs::init() {}

  void VulkanJobs::terminate()
  {
    ESP_ASSERT(VulkanJobs::s_instance != nullptr, "VulkanJobs is deleted twice!");

    s_instance = nullptr;
  }

  void VulkanJobs::done_all_jobs() { vkDeviceWaitIdle(VulkanDevice::get_logical_device()); }

  void VulkanJobs::draw(uint32_t vertex_count)
  {
    vkCmdDraw(VulkanWorkOrchestrator::get_current_command_buffer(), vertex_count, 1, 0, 0);
  }

  void VulkanJobs::draw(uint32_t vertex_count, uint32_t instance_count)
  {
    vkCmdDraw(VulkanWorkOrchestrator::get_current_command_buffer(), vertex_count, instance_count, 0, 0);
  }

  void VulkanJobs::draw_indexed(uint32_t index_count)
  {
    vkCmdDrawIndexed(VulkanWorkOrchestrator::get_current_command_buffer(), index_count, 1, 0, 0, 0);
  }

  void VulkanJobs::draw_indexed(uint32_t index_count, uint32_t instance_count)
  {
    vkCmdDrawIndexed(VulkanWorkOrchestrator::get_current_command_buffer(), index_count, instance_count, 0, 0, 0);
  }
} // namespace esp
