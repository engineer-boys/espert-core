#ifndef RENDERER_ESP_DESCRIPTORS_H_
#define RENDERER_ESP_DESCRIPTORS_H_

#include "EspDevice.hh"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace esp
{
	class EspDescriptorSetLayout
	{
	 public:
		class Builder
		{
		 private:
			EspDevice& espDevice;
			std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};

		 public:
			Builder(EspDevice& device) : espDevice{ device }
			{
			}

			Builder& addBinding(
				uint32_t binding,
				VkDescriptorType descriptorType,
				VkShaderStageFlags stageFlags,
				uint32_t count = 1);
			std::unique_ptr<EspDescriptorSetLayout> build() const;
		};

	 private:
		EspDevice& espDevice;
		VkDescriptorSetLayout descriptorSetLayout;
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

	 public:
		EspDescriptorSetLayout(
			EspDevice& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
		~EspDescriptorSetLayout();

		EspDescriptorSetLayout(const EspDescriptorSetLayout&) = delete;
		EspDescriptorSetLayout& operator=(const EspDescriptorSetLayout&) = delete;

		VkDescriptorSetLayout getDescriptorSetLayout() const
		{
			return descriptorSetLayout;
		}

		friend class EspDescriptorWriter;
	};

	class EspDescriptorPool
	{
	 public:
		class Builder
		{
		 private:
			EspDevice& espDevice;
			std::vector<VkDescriptorPoolSize> poolSizes{};
			uint32_t maxSets = 1000;
			VkDescriptorPoolCreateFlags poolFlags = 0;

		 public:
			Builder(EspDevice& device) : espDevice{ device }
			{
			}

			Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
			Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
			Builder& setMaxSets(uint32_t count);
			std::unique_ptr<EspDescriptorPool> build() const;
		};

	 private:
		EspDevice& espDevice;
		VkDescriptorPool descriptorPool;

	 public:
		EspDescriptorPool(
			EspDevice& device,
			uint32_t maxSets,
			VkDescriptorPoolCreateFlags poolFlags,
			const std::vector<VkDescriptorPoolSize>& poolSizes);
		~EspDescriptorPool();
		EspDescriptorPool(const EspDescriptorPool&) = delete;
		EspDescriptorPool& operator=(const EspDescriptorPool&) = delete;

		bool allocateDescriptorSet(
			const VkDescriptorSetLayout& descriptorSetLayout, VkDescriptorSet& descriptor) const;

		void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

		void resetPool();

		friend class EspDescriptorWriter;
	};

	class EspDescriptorWriter
	{
	 private:
		EspDescriptorSetLayout& setLayout;
		EspDescriptorPool& pool;
		std::vector<VkWriteDescriptorSet> writes;

	 public:
		EspDescriptorWriter(EspDescriptorSetLayout& setLayout, EspDescriptorPool& pool);

		EspDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
		EspDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

		bool build(VkDescriptorSet& set);
		void overwrite(VkDescriptorSet& set);
	};
}

#endif //RENDERER_ESP_DESCRIPTORS_H_
