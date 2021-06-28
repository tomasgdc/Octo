#include "Vulkan/VkGPUMemoryManager.h"
#include "Vulkan/VkRenderSystem.h"
#include "Vulkan/VulkanTools.h"

//Other
#include <cassert>

namespace Renderer
{
	namespace Vulkan
	{
		std::vector<GpuMemoryPage> GpuMemoryManager::memoryPools[MemoryPoolTypes::kCount];
		MemoryLocation::Enum GpuMemoryManager::memoryPoolToMemoryLocation[MemoryPoolTypes::kCount] = {};
		uint32_t GpuMemoryManager::memoryLocationToMemoryPropertyFlags[MemoryLocation::kCount] =
		{
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		void GpuMemoryManager::Init()
		{
			memoryPoolToMemoryLocation[MemoryPoolTypes::kStaticImages] = MemoryLocation::kDeviceLocal;
			memoryPoolToMemoryLocation[MemoryPoolTypes::kStaticBuffers] = MemoryLocation::kDeviceLocal;
			memoryPoolToMemoryLocation[MemoryPoolTypes::kStaticStagingBuffers] = MemoryLocation::kHostVisible;
			memoryPoolToMemoryLocation[MemoryPoolTypes::kResolutionDependentBuffers] = MemoryLocation::kDeviceLocal;
			memoryPoolToMemoryLocation[MemoryPoolTypes::kResolutionDependentImages] = MemoryLocation::kDeviceLocal;
			memoryPoolToMemoryLocation[MemoryPoolTypes::kResolutionDependentStagingBuffers] = MemoryLocation::kHostVisible;
			memoryPoolToMemoryLocation[MemoryPoolTypes::kVolatileStagingBuffers] = MemoryLocation::kHostVisible;
		}

		void GpuMemoryManager::Destroy()
		{

		}

		MemoryPoolTypes::GpuMemoryAllocationInfo GpuMemoryManager::AllocateOffset(MemoryPoolTypes::Enum poolType, uint32_t size, uint32_t allignement, uint32_t memoryFlags)
		{
			std::vector<GpuMemoryPage>& poolPages = memoryPools[poolType];

			for (uint32_t pageIdX = 0u; pageIdX < memoryPools[poolType].size(); pageIdX++)
			{
				GpuMemoryPage& page = poolPages[pageIdX];

				if (memoryFlags & (1u << page._memoryTypeIdx) > 0u && page.allocator.Fits(size, allignement))
				{
					void* ptr = page.allocator.Allocate(size, allignement);
					const uint32_t offset = page.allocator.GetOffset();

					return { poolType, pageIdX, offset, page._vkDeviceMemory, size,
							allignement, page._mappedMemory != nullptr ? &page._mappedMemory[offset]: nullptr };
				}
			}

			for (uint32_t memoryTypeIndex = 0; memoryTypeIndex < RenderSystem::vkPhysicalDeviceMemoryProperties.memoryTypeCount; ++memoryTypeIndex)
			{
				const VkMemoryType& memoryType = RenderSystem::vkPhysicalDeviceMemoryProperties.memoryTypes[memoryTypeIndex];
				const MemoryLocation::Enum memoryLocation = memoryPoolToMemoryLocation[poolType];
				const uint32_t memoryPropertyFlag = memoryLocationToMemoryPropertyFlags[memoryLocation];

				if (memoryPropertyFlag & memoryType.propertyFlags && (memoryFlags & (1u << memoryTypeIndex)) > 0u)
				{
					poolPages.resize(poolPages.size() + 1u);
					GpuMemoryPage& page = poolPages.back();

					page.allocator.Init(OCTO_GPU_PAGE_SIZE_IN_BYTES);
					page._memoryTypeIdx = memoryTypeIndex;

					VkMemoryAllocateInfo memAllocInfo = {};
					{
						memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
						memAllocInfo.pNext = 0u;
						memAllocInfo.allocationSize = OCTO_GPU_PAGE_SIZE_IN_BYTES;
						memAllocInfo.memoryTypeIndex = memoryTypeIndex;
					}

					VkResult result = vkAllocateMemory(RenderSystem::vkDevice, &memAllocInfo, nullptr, &page._vkDeviceMemory);
					VK_CHECK_RESULT(result);

					if (memoryLocation == MemoryLocation::kHostVisible)
					{
						VkResult result = vkMapMemory(RenderSystem::vkDevice, page._vkDeviceMemory, 0u, OCTO_GPU_PAGE_SIZE_IN_BYTES, 0u, (void**)&page._mappedMemory);
						VK_CHECK_RESULT(result);
					}

					//assert(page.allocator.Fits(size, ) && "Allocation does not fit in a single page");
					void* ptr = page.allocator.Allocate(size, allignement);
					const uint32_t offset = page.allocator.GetOffset();

					return { poolType, static_cast<uint32_t>(poolPages.size() - 1u), offset, page._vkDeviceMemory, size, allignement,
					page._mappedMemory != nullptr ? &page._mappedMemory[offset] : nullptr};
				}
			}
		}
	}
}