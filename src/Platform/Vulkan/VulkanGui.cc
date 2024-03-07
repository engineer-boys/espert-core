#include "VulkanGui.hh"
#include "Core/EspWindow.hh"
#include "VulkanContext.hh"
#include "VulkanDevice.hh"
#include "Work/VulkanWorkOrchestrator.hh"

#define IMGUI_DESCRIPTOR_POOL_MAX_SETS 100

namespace esp
{
  VulkanGui* VulkanGui::s_instance = nullptr;

  std::unique_ptr<VulkanGui> VulkanGui::create()
  {
    ESP_ASSERT(VulkanGui::s_instance == nullptr, "Vulkan Gui already exists!");
    VulkanGui::s_instance = new VulkanGui();
    VulkanGui::s_instance->init();

    return std::unique_ptr<VulkanGui>{ VulkanGui::s_instance };
  }

  void VulkanGui::terminate()
  {
    ImGui_ImplVulkan_Shutdown();
    vkDestroyDescriptorPool(VulkanDevice::get_logical_device(), s_instance->m_desc_pool, nullptr);
  }

  void VulkanGui::new_frame()
  {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
  }

  void VulkanGui::begin() { ImGui::Begin(s_instance->m_name.c_str()); }

  void VulkanGui::end() { ImGui::End(); }

  void VulkanGui::end_frame() { ImGui::Render(); }

  void VulkanGui::render()
  {
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), VulkanWorkOrchestrator::get_current_command_buffer());
  }

  void VulkanGui::init()
  {
    // [1] create descriptor pool for IMGUI
    VkDescriptorPoolSize pool_sizes[] = { { VK_DESCRIPTOR_TYPE_SAMPLER, IMGUI_DESCRIPTOR_POOL_MAX_SETS },
                                          { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, IMGUI_DESCRIPTOR_POOL_MAX_SETS },
                                          { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, IMGUI_DESCRIPTOR_POOL_MAX_SETS },
                                          { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, IMGUI_DESCRIPTOR_POOL_MAX_SETS },
                                          { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, IMGUI_DESCRIPTOR_POOL_MAX_SETS },
                                          { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, IMGUI_DESCRIPTOR_POOL_MAX_SETS },
                                          { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, IMGUI_DESCRIPTOR_POOL_MAX_SETS },
                                          { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, IMGUI_DESCRIPTOR_POOL_MAX_SETS },
                                          { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, IMGUI_DESCRIPTOR_POOL_MAX_SETS },
                                          { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, IMGUI_DESCRIPTOR_POOL_MAX_SETS },
                                          { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, IMGUI_DESCRIPTOR_POOL_MAX_SETS } };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags                      = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets                    = IMGUI_DESCRIPTOR_POOL_MAX_SETS;
    pool_info.poolSizeCount              = std::size(pool_sizes);
    pool_info.pPoolSizes                 = pool_sizes;

    vkCreateDescriptorPool(VulkanDevice::get_logical_device(), &pool_info, nullptr, &m_desc_pool); // TODO: if for error

    // [2] initialize IMGUI library

    // - initialize core IMGUI structures
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // - initialize IMGUI for GLFW
    ImGui_ImplGlfw_InitForVulkan(EspWindow::get_window(), true);

    auto& context_data = VulkanContext::get_context_data();
    // - initialize IMGUI for Vulkan
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance                  = context_data.m_instance;
    init_info.PhysicalDevice            = VulkanDevice::get_physical_device();
    init_info.Device                    = VulkanDevice::get_logical_device();
    init_info.QueueFamily = context_data.m_queue_family_indices.m_graphics_family.value(); // TODO: present_family (?)
    init_info.Queue       = context_data.m_graphics_queue;                                 // TODO: present_queue (?)
    init_info.DescriptorPool        = m_desc_pool;
    init_info.Subpass               = 0;
    init_info.MinImageCount         = 3;
    init_info.ImageCount            = 3;
    init_info.MSAASamples           = VK_SAMPLE_COUNT_1_BIT;
    init_info.UseDynamicRendering   = true;
    init_info.ColorAttachmentFormat = *VulkanSwapChain::get_swap_chain_image_format();

    // - load necessary lambda for handling Volk
    ImGui_ImplVulkan_LoadFunctions(
        [](const char* func_name, void*)
        {
          PFN_vkVoidFunction instance_addr =
              vkGetInstanceProcAddr(VulkanContext::get_context_data().m_instance, func_name);
          PFN_vkVoidFunction device_addr = vkGetDeviceProcAddr(VulkanDevice::get_logical_device(), func_name);
          return device_addr ? device_addr : instance_addr;
        });

    ImGui_ImplVulkan_Init(&init_info, nullptr);
  }

} // namespace esp