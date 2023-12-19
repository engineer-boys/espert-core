#include "VulkanJob.hh"
#include "Platform/Vulkan/VulkanDevice.hh"
#include "VulkanWorkOrchestrator.hh"

namespace esp
{
  VulkanJob* VulkanJob::s_instance = nullptr;

  std::unique_ptr<VulkanJob> VulkanJob::create()
  {
    ESP_ASSERT(VulkanJob::s_instance == nullptr, "VulkanJob already exists!");
    VulkanJob::s_instance = new VulkanJob();
    VulkanJob::s_instance->init();

    return std::unique_ptr<VulkanJob>{ VulkanJob::s_instance };
  }

  VulkanJob::VulkanJob() {}

  VulkanJob::~VulkanJob() {}

  void VulkanJob::init() {}

  void VulkanJob::terminate()
  {
    ESP_ASSERT(VulkanJob::s_instance != nullptr, "VulkanJob is deleted twice!");

    s_instance = nullptr;
  }

  void VulkanJob::done_all_jobs() { vkDeviceWaitIdle(VulkanDevice::get_logical_device()); }

  void VulkanJob::draw(uint32_t vertex_count)
  {
    vkCmdDraw(VulkanWorkOrchestrator::get_current_command_buffer(), vertex_count, 1, 0, 0);
  }

  void VulkanJob::draw(uint32_t vertex_count, uint32_t instance_count)
  {
    vkCmdDraw(VulkanWorkOrchestrator::get_current_command_buffer(), vertex_count, instance_count, 0, 0);
  }

  void VulkanJob::draw_indexed(uint32_t index_count)
  {
    vkCmdDrawIndexed(VulkanWorkOrchestrator::get_current_command_buffer(), index_count, 1, 0, 0, 0);
  }

  void VulkanJob::draw_indexed(uint32_t index_count, uint32_t instance_count)
  {
    vkCmdDrawIndexed(VulkanWorkOrchestrator::get_current_command_buffer(), index_count, instance_count, 0, 0, 0);
  }
} // namespace esp
