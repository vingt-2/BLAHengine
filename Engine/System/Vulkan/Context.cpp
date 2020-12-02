#include "Context.h"
#include "StdInclude.h"
#include "System.h"

namespace BLA::System::Vulkan
{
    Context::Context(const char** extensions, uint32_t extensions_count)
    {
        VkResult err;
        // Create Vulkan Instance
        {
            VkInstanceCreateInfo create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            create_info.enabledExtensionCount = extensions_count;
            create_info.ppEnabledExtensionNames = extensions;

            // Create Vulkan Instance without any debug feature
            err = vkCreateInstance(&create_info, m_allocator, &m_instance);
            HandleError(err);
        }

        // Select GPU
        {
            uint32_t gpu_count;
            err = vkEnumeratePhysicalDevices(m_instance, &gpu_count, NULL);
            HandleError(err);
            BLA_ASSERT(gpu_count > 0);

            VkPhysicalDevice* gpus = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * gpu_count);
            err = vkEnumeratePhysicalDevices(m_instance, &gpu_count, gpus);
            HandleError(err);

            // If a number >1 of GPUs got reported, you should find the best fit GPU for your purpose
            // e.g. VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU if available, or with the greatest memory available, etc.
            // for sake of simplicity we'll just take the first one, assuming it has a graphics queue family.
            m_physicalDevice = gpus[0];
            free(gpus);
        }

        // Select graphics queue family
        {
            uint32_t count;
            vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &count, NULL);
            VkQueueFamilyProperties* queues = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * count);
            vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &count, queues);
            for (uint32_t i = 0; i < count; i++)
                if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                {
                    m_queueFamily = i;
                    break;
                }
            free(queues);
            BLA_ASSERT(m_queueFamily != (blaU32)-1);
        }

        {
            vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &m_memoryProperties);
        }

        // Create Logical Device (with 1 queue)
        {
            int device_extension_count = 1;
            const char* device_extensions[] = { "VK_KHR_swapchain" };
            const float queue_priority[] = { 1.0f };
            VkDeviceQueueCreateInfo queue_info[1] = {};
            queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_info[0].queueFamilyIndex = m_queueFamily;
            queue_info[0].queueCount = 1;
            queue_info[0].pQueuePriorities = queue_priority;
            VkDeviceCreateInfo create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
            create_info.pQueueCreateInfos = queue_info;
            create_info.enabledExtensionCount = device_extension_count;
            create_info.ppEnabledExtensionNames = device_extensions;
            err = vkCreateDevice(m_physicalDevice, &create_info, m_allocator, &m_device);
            HandleError(err);
            vkGetDeviceQueue(m_device, m_queueFamily, 0, &m_queue);
        }

        // Create Descriptor Pool
        {
            VkDescriptorPoolSize pool_sizes[] =
            {
                { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
                { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
                { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
            };
            VkDescriptorPoolCreateInfo pool_info = {};
            pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            pool_info.maxSets = 1000 * ArraySize(pool_sizes);
            pool_info.poolSizeCount = (uint32_t)ArraySize(pool_sizes);
            pool_info.pPoolSizes = pool_sizes;
            err = vkCreateDescriptorPool(m_device, &pool_info, m_allocator, &m_descriptorPool);
            HandleError(err);
        }

        // Create logisitcs command pool
        VkCommandPoolCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        info.queueFamilyIndex = m_queueFamily;
        err = vkCreateCommandPool(m_device, &info, nullptr, &m_TransferCommandPool);
        HandleError(err);

        m_currentCommandPool = m_TransferCommandPool;
    }

    uint32_t Vulkan::Context::GetMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32* memTypeFound) const
    {
        for (uint32_t i = 0; i < m_memoryProperties.memoryTypeCount; i++)
        {
            if ((typeBits & 1) == 1)
            {
                if ((m_memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
                {
                    if (memTypeFound)
                    {
                        *memTypeFound = true;
                    }
                    return i;
                }
            }
            typeBits >>= 1;
        }

        if (memTypeFound)
        {
            *memTypeFound = false;
            return 0;
        }

        return -1;
    }

    void Context::HandleError(VkResult err)
    {
        if (err == 0) return;
        printf("Vulkan Error: %i\n", err);
        if (err < 0)
            abort();
    }
}
