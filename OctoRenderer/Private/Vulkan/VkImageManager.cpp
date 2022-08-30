#include "Vulkan/VkImageManager.h"
#include "Vulkan/VkRenderSystem.h"
#include "Vulkan/VkGPUMemoryManager.h"
#include "Vulkan/VulkanTools.h"

namespace
{
	void AllocateMemory(MemoryPoolTypes::GpuMemoryAllocationInfo& memAllocInfo, MemoryPoolTypes::Enum poolType, const VkMemoryRequirements memRegs)
	{
		bool bNeedsAlloc = true;
		
		if(poolType >= MemoryPoolTypes::kRangeStartStatic && poolType <= MemoryPoolTypes::kRangeEndStatic)
		{ 
			if (memAllocInfo._sizeInBytes <= memRegs.size && memAllocInfo._alignmentInBytes == memRegs.alignment && memAllocInfo._memoryPoolType == poolType)
			{
				bNeedsAlloc = false;
			}
		}

		if (bNeedsAlloc)
		{
			memAllocInfo = Renderer::Vulkan::GpuMemoryManager::AllocateOffset(poolType, memRegs.size, memRegs.alignment, memRegs.memoryTypeBits);
		}
	}
}
namespace Renderer
{
	namespace Resource
	{
		void ImageManager::CreateResource(const DOD::Ref p_Images)
		{
			ImageType::Enum imageType  = ImageManager::GetImageType(p_Images);

			if (imageType == ImageType::Enum::kTexture)
				CreateTexture(p_Images);
		}

		void ImageManager::CreateTexture(const DOD::Ref ref)
		{
			VkImage& image = ImageManager::GetVkImage(ref);
			VkFormat& imageFormat = ImageManager::GetImageFormat(ref);
			glm::uvec3& dimensions = ImageManager::GetImageDimensions(ref);
			uint8_t imageFlags = ImageManager::GetImageFlags(ref);
			const uint32_t arrayLayerCount = ImageManager::GetArrayLayerCount(ref);
			const uint32_t mipLevelCount = ImageManager::GetMipLevelCount(ref);
			const MemoryPoolTypes::Enum memoryPoolType = ImageManager::GetMemoryPoolType(ref);
			MemoryPoolTypes::GpuMemoryAllocationInfo& memoryAllocationInfo = ImageManager::GetMemoryAllocationInfo(ref);

			assert(dimensions.x >= 1.0f && dimensions.y >= 1.0f && dimensions.z >= 1.0f);

			VkImageType vkImageType = VK_IMAGE_TYPE_1D;
			VkImageViewType vkImageViewTypeSubResource = VK_IMAGE_VIEW_TYPE_1D;
			VkImageViewType vkImageViewType = arrayLayerCount == 1u ? VK_IMAGE_VIEW_TYPE_1D : VK_IMAGE_VIEW_TYPE_1D_ARRAY;

			bool isDepthTarget = false;
			bool isStencilTarget = false;

			if (dimensions.y >= 2.0f && dimensions.z == 1.0f)
			{
				vkImageType = VK_IMAGE_TYPE_2D;
				vkImageViewTypeSubResource = VK_IMAGE_VIEW_TYPE_2D;
				vkImageViewType = arrayLayerCount == 1u ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_2D_ARRAY;
				ImageTextureType::Enum& textureType = ImageManager::GetImageTextureType(ref);
				textureType =  arrayLayerCount == 1u ? ImageTextureType::k2D : ImageTextureType::k2DArray;
			}
			else if (dimensions.y >= 2.0f && dimensions.z >= 2.0f)
			{
				assert(arrayLayerCount == 1u);
				vkImageType = VK_IMAGE_TYPE_3D;
				vkImageViewTypeSubResource = VK_IMAGE_VIEW_TYPE_3D;
				vkImageViewType = VK_IMAGE_VIEW_TYPE_3D;
				ImageTextureType::Enum& textureType = ImageManager::GetImageTextureType(ref);
				textureType = arrayLayerCount == 1u ? ImageTextureType::k2D : ImageTextureType::k2DArray;
			}

			VkImageCreateInfo imageCreateInfo = {};

			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(Renderer::Vulkan::RenderSystem::vkPhysicalDevice, imageFormat, &props);

			imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;

			if(imageFlags & ImageFlags::kUsageAttachment)
			{ 
				if(isDepthTarget || isStencilTarget)
				{
					if (props.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
					{
						imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
					}
					else if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
					{
						imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
					}
					else
					{
						assert(false && "Depth/stencil format not supported");
					}
				}
			}
			else
			{
				if (props.linearTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT)
				{
					imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
				}
				else if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT)
				{
					imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
				}
				else
				{
					assert(false && "Color format not supported");
				}
			}

			if (imageFlags & ImageFlags::kUsageSampled > 0u)
			{

			}

			if (imageFlags & ImageFlags::kUsageStorage > 0u)
			{

			}

			imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageCreateInfo.pNext = nullptr;
			imageCreateInfo.imageType = vkImageType;
			imageCreateInfo.format = imageFormat;
			imageCreateInfo.extent.width = (uint32_t)dimensions.x;
			imageCreateInfo.extent.height = (uint32_t)dimensions.y;
			imageCreateInfo.extent.depth = (uint32_t)dimensions.z;
			imageCreateInfo.mipLevels = mipLevelCount;
			imageCreateInfo.arrayLayers = arrayLayerCount;
			imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageCreateInfo.queueFamilyIndexCount = 0u;
			imageCreateInfo.pQueueFamilyIndices = nullptr;
			imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			// Setup usage
			imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			if (imageFlags & ImageFlags::kUsageAttachment > 0)
			{
				imageCreateInfo.usage |= (isDepthTarget || isStencilTarget) ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			}

			if (imageFlags & ImageFlags::kUsageSampled > 0u)
			{
				imageCreateInfo.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
			}

			if (imageFlags & ImageFlags::kUsageStorage > 0u)
			{
				imageCreateInfo.usage |= VK_IMAGE_USAGE_STORAGE_BIT;
			}

			imageCreateInfo.flags = 0u;

			VK_CHECK_RESULT(vkCreateImage(Renderer::Vulkan::RenderSystem::vkDevice, &imageCreateInfo, nullptr, &image));

			VkMemoryRequirements memReqs;
			vkGetImageMemoryRequirements(Renderer::Vulkan::RenderSystem::vkDevice, image, &memReqs);

			AllocateMemory(memoryAllocationInfo, memoryPoolType, memReqs);
			VK_CHECK_RESULT(vkBindImageMemory(Renderer::Vulkan::RenderSystem::vkDevice, image, memoryAllocationInfo._vkDeviceMemory, memoryAllocationInfo._offset));

			VkImageViewCreateInfo imageViewCreateInfo = {};
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.pNext = nullptr;
			imageViewCreateInfo.image = VK_NULL_HANDLE;
			imageViewCreateInfo.format = imageFormat;
			imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
			imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
			imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
			imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;

			imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewCreateInfo.flags = 0;
			imageViewCreateInfo.image = image;

			ImageViewArray& imageViewArray = ImageManager::GetSubresourceImageViews(ref);
			imageViewArray.resize(arrayLayerCount);

			for (uint32_t arrayLayerIdx = 0u; arrayLayerIdx < arrayLayerCount; ++arrayLayerIdx)
			{
				imageViewArray[arrayLayerIdx].resize(mipLevelCount);
			}

			// Image views for each sub resource
			for (uint32_t arrayLayerIdx = 0u; arrayLayerIdx < arrayLayerCount; ++arrayLayerIdx)
			{
				for (uint32_t mipLevelIdx = 0u; mipLevelIdx < mipLevelCount; ++mipLevelIdx)
				{
					imageViewCreateInfo.viewType = vkImageViewTypeSubResource;
					imageViewCreateInfo.subresourceRange.baseMipLevel = mipLevelIdx;
					imageViewCreateInfo.subresourceRange.levelCount = 1u;
					imageViewCreateInfo.subresourceRange.baseArrayLayer = arrayLayerIdx;
					imageViewCreateInfo.subresourceRange.layerCount = 1u;

					VK_CHECK_RESULT(vkCreateImageView(Renderer::Vulkan::RenderSystem::vkDevice, &imageViewCreateInfo, nullptr, &imageViewArray[arrayLayerIdx][mipLevelIdx]));
				}
			}

			// General image view
			imageViewCreateInfo.viewType = vkImageViewType;
			imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imageViewCreateInfo.subresourceRange.levelCount = mipLevelCount;
			imageViewCreateInfo.subresourceRange.baseArrayLayer = 0u;
			imageViewCreateInfo.subresourceRange.layerCount = arrayLayerCount;

			VK_CHECK_RESULT(vkCreateImageView(Renderer::Vulkan::RenderSystem::vkDevice, &imageViewCreateInfo, nullptr, &ImageManager::GetImageView(ref)));
		}


		void ImageManager::DestroyResource(const std::vector<DOD::Ref>& refs)
		{
			for (auto& ref : refs)
			{
				VkImage& image = GetVkImage(ref);
				auto& imageViews = GetSubresourceImageViews(ref);
				VkImageView imageView = GetImageView(ref);

				if (!HasImageFlags(ref, ImageFlags::kExternalImage))
				{
					if (image != VK_NULL_HANDLE)
					{
						vkDestroyImage(Renderer::Vulkan::RenderSystem::vkDevice, image, nullptr);
					}
				}
				image = VK_NULL_HANDLE;

				if (!HasImageFlags(ref, ImageFlags::kExternalView))
				{
					for (uint32_t arrayLayerIdx = 0u; arrayLayerIdx < imageViews.size(); ++arrayLayerIdx)
					{
						for (uint32_t mipIdx = 0u; mipIdx < imageViews[arrayLayerIdx].size(); ++mipIdx)
						{
							VkImageView vkImageView = imageViews[arrayLayerIdx][mipIdx];
							if (vkImageView != VK_NULL_HANDLE)
							{
								vkDestroyImageView(Renderer::Vulkan::RenderSystem::vkDevice, vkImageView, nullptr);
								vkImageView = VK_NULL_HANDLE;
							}
						}

						if (imageView != VK_NULL_HANDLE)
						{
							vkDestroyImageView(Renderer::Vulkan::RenderSystem::vkDevice, imageView, nullptr);
							imageView = VK_NULL_HANDLE;
						}
					}
				}
				imageViews.clear();
			}
		}

		void ImageManager::InsertImageMemoryBarrier(VkCommandBuffer commandBuffer, DOD::Ref imageRef,
			VkImageLayout srcImageLayout, VkImageLayout dstImageLayout,
			VkPipelineStageFlags srcStages,
			VkPipelineStageFlags dstStages)
		{
			VkImageSubresourceRange range;
			range.aspectMask = GetImageFormat(imageRef) != Renderer::Vulkan::RenderSystem::vkDepthFormatToUse ? VK_IMAGE_ASPECT_COLOR_BIT : (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);

			range.baseMipLevel = 0u;
			range.levelCount = GetMipLevelCount(imageRef);
			range.baseArrayLayer = 0u;
			range.layerCount = GetArrayLayerCount(imageRef);

			VkTools::InsertImageMemoryBarrier(commandBuffer, GetVkImage(imageRef), srcImageLayout, dstImageLayout, range, srcStages, dstStages);
		}
	}
}
