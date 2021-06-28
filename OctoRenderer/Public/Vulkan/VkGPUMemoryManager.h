#pragma once
#include <vector>
#include "ThirdParty\vulkan\vulkan.h"
#include "VkEnums.h"

//Core
#include "OctoCore/Public/LinearAllocator.h"

#define OCTO_GPU_PAGE_SIZE_IN_BYTES (80u * 1024u * 1024u)

namespace Renderer
{
	namespace Vulkan
	{
		struct GpuMemoryPage
		{
			Core::Memory::LinearAllocator allocator;
			VkDeviceMemory _vkDeviceMemory;
			uint8_t* _mappedMemory;
			uint32_t _memoryTypeIdx;
		};

		struct GpuMemoryManager
		{
			static void Init();
			static void Destroy();
			static MemoryPoolTypes::GpuMemoryAllocationInfo AllocateOffset(MemoryPoolTypes::Enum poolType, uint32_t size, uint32_t allignement, uint32_t memoryFlags);

		private:
			static std::vector<GpuMemoryPage> memoryPools[MemoryPoolTypes::kCount];
			static MemoryLocation::Enum memoryPoolToMemoryLocation[MemoryPoolTypes::kCount];
			static uint32_t memoryLocationToMemoryPropertyFlags[MemoryLocation::kCount];
		};
	}
}