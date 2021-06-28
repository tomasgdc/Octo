#pragma once
//ThirdParty
#include "ThirdParty\vulkan\vulkan.h"

//Other
#include <cstdint>
#include <cstddef>

namespace AttachementFlags
{
	enum Enum
	{
		kClearOnLoad = 0x01u,
		kClearStencilOnLoad = 0x02u
	};
}

struct AttachementDescription
{
	VkFormat format;
	uint8_t flags;
	bool  depthFormat;
};

namespace ImageFlags
{
	enum Enum
	{
		kExternalImage = 0x01u,
		kExternalView = 0x02u,
		kExternalDeviceMemory = 0x04u,

		kUsageAttachment = 0x08u,
		kUsageSampled = 0x10u,
		kUsageStorage = 0x20u
	};
};
namespace ImageType
{
	enum class Enum
	{
		kTexture,
		kTextureFromFile
	};
}

namespace ImageTextureType
{
	enum Enum
	{
		kUnknown,

		k1D,
		k1DArray,
		k2D,
		k2DArray,
		k3D,
		k3DArray,

		kCube,
		kCubeArray
	};
}

namespace MemoryPoolTypes
{ 
	enum Enum
	{
		kStaticImages,
		kStaticBuffers,
		kStaticStagingBuffers,

		kResolutionDependentImages,
		kResolutionDependentBuffers,
		kResolutionDependentStagingBuffers,

		kVolatileStagingBuffers,

		kCount,

		kRangeStartStatic = kStaticImages,
		kRangeEndStatic = kStaticStagingBuffers,
		kRangeStartResolutionDependent = kResolutionDependentImages,
		kRangeEndResolutionDependent = kResolutionDependentStagingBuffers,
		kRangeStartVolatile = kVolatileStagingBuffers,
		kRangeEndVolatile = kVolatileStagingBuffers
	};

	struct GpuMemoryAllocationInfo
	{
		MemoryPoolTypes::Enum _memoryPoolType;
		uint32_t _pageIdx;
		std::size_t _offset;
		VkDeviceMemory _vkDeviceMemory;
		uint32_t _sizeInBytes;
		uint32_t _alignmentInBytes;
		uint8_t* _mappedMemory;
	};
};


namespace MemoryLocation
{
	enum Enum
	{
		kDeviceLocal,
		kHostVisible,
		kCount
	};
};