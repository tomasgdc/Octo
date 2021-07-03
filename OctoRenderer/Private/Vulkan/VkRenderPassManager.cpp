#include "Vulkan/VkRenderPassManager.h"
#include "Vulkan/VulkanTools.h"
#include "Vulkan/VkRenderSystem.h"

namespace Renderer
{
	namespace Resource
	{
		void RenderPassManager::CreateResource(const std::vector<DOD::Ref>& refs)
		{
			for (const auto& ref : refs)
			{
				VkRenderPass& render_pass = RenderPassManager::GetRenderPass(ref);
				std::vector<AttachementDescription>& attachements = RenderPassManager::GetAttachementDescription(ref);

				std::vector<VkAttachmentDescription> attachementsDescriptions;
				attachementsDescriptions.reserve(attachements.size());

				std::vector<VkAttachmentReference> colorRefs;
				std::vector<VkAttachmentReference> depthRefs;

				for (uint32_t i = 0; i < attachements.size(); i++)
				{
					AttachementDescription& attachement = attachements[i];

					VkImageLayout imageLayout = !attachement.depthFormat ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

					VkAttachmentDescription attachmentDesc = {};
					attachmentDesc.format = attachement.format;
					attachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
					attachmentDesc.loadOp = (attachement.flags & AttachementFlags::kClearOnLoad) > 0u
						? VK_ATTACHMENT_LOAD_OP_CLEAR
						: VK_ATTACHMENT_LOAD_OP_DONT_CARE;

					attachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
					attachmentDesc.stencilLoadOp = (attachement.flags & AttachementFlags::kClearStencilOnLoad) > 0u
						? VK_ATTACHMENT_LOAD_OP_CLEAR
						: VK_ATTACHMENT_LOAD_OP_DONT_CARE;

					attachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
					attachmentDesc.initialLayout = imageLayout;
					attachmentDesc.finalLayout = imageLayout;
					attachmentDesc.flags = 0u;

					attachementsDescriptions.push_back(std::move(attachmentDesc));

					// Create refs.
					VkAttachmentReference attachmentRef = {};
					{
						attachmentRef.attachment = i;
						attachmentRef.layout = imageLayout;
					}

					if (imageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
					{
						colorRefs.push_back(attachmentRef);
					}
					else if (imageLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
					{
						depthRefs.push_back(attachmentRef);
					}
				}


				VkSubpassDescription subpass = {};
				subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
				subpass.flags = 0;
				subpass.inputAttachmentCount = 0;
				subpass.pInputAttachments = nullptr;
				subpass.colorAttachmentCount = colorRefs.size();
				subpass.pColorAttachments = !colorRefs.empty() ? colorRefs.data() : nullptr;
				subpass.pResolveAttachments = nullptr;
				subpass.pDepthStencilAttachment = !depthRefs.empty() ? depthRefs.data() : nullptr;
				subpass.preserveAttachmentCount = 0;
				subpass.pPreserveAttachments = nullptr;

				VkSubpassDependency dependency = { 0 };
				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.srcAccessMask = 0;
				dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

				VkRenderPassCreateInfo renderPassInfo = {};
				renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
				renderPassInfo.pNext = nullptr;
				renderPassInfo.attachmentCount = attachementsDescriptions.size();
				renderPassInfo.pAttachments = attachementsDescriptions.data();
				renderPassInfo.subpassCount = 1;
				renderPassInfo.pSubpasses = &subpass;
				renderPassInfo.dependencyCount = 1;
				renderPassInfo.pDependencies = &dependency;
				renderPassInfo.flags = 0u;

				VK_CHECK_RESULT(vkCreateRenderPass(Vulkan::RenderSystem::vkDevice, &renderPassInfo, nullptr, &render_pass));
			}
		}

		void RenderPassManager::DestroyResources(const std::vector<DOD::Ref>& refs)
		{
			for (uint32_t i = 0; i < refs.size(); i++)
			{
				DOD::Ref ref = refs[i];
				VkRenderPass& render_pass = RenderPassManager::GetRenderPass(ref);

				if (render_pass != VK_NULL_HANDLE)
				{
					vkDestroyRenderPass(Vulkan::RenderSystem::vkDevice, render_pass, nullptr);
					render_pass = VK_NULL_HANDLE;
				}
			}
		}
	}
}