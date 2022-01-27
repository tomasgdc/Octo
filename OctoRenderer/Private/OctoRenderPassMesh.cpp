#include "OctoRenderPassMesh.h"
#include "Vulkan\VkRenderPassManager.h"
#include "Vulkan\VkGPUMemoryManager.h"
#include "Vulkan\VkPipelineLayoutManager.h"
#include "Vulkan\VkFrameBufferManager.h"
#include "Vulkan\VkBufferLayoutManager.h"
#include "Vulkan\VkImageManager.h"
#include "Vulkan\VkRenderSystem.h"
#include "Vulkan\VkGpuProgram.h"
#include "Vulkan\VulkanTools.h"
#include "Vulkan\VkPipelineManager.h"
#include "Vulkan\DrawCallManager.h"
#include "Vulkan\VkDrawCallDispatcher.h"
#include "Vulkan\VkBufferObjectManager.h"
#include "Vulkan\VkUniformBufferManager.h"
#include "Geometry\VertData.h"

//ThirdParty
#include <ThirdParty/glm/glm/glm.hpp>
#include <ThirdParty/glm/glm/gtc/matrix_transform.hpp>
#include <ThirdParty/glm/glm/gtc/type_ptr.hpp>
#include <ThirdParty/glm/glm/gtc/matrix_inverse.hpp>
#include <ThirdParty/glm/glm/gtx/quaternion.hpp> 
#include <ThirdParty/glm/glm/gtx/compatibility.hpp> 
#include <ThirdParty/glm/glm/gtx/euler_angles.hpp>
#include <ThirdParty/glm/glm/gtx/norm.hpp>
#include <ThirdParty/glm/glm/gtx/transform.hpp>
#include <ThirdParty/glm/glm/gtx/matrix_operation.hpp>
#include <ThirdParty\glm\glm\gtx\orthonormalize.hpp>

glm::vec3	g_Rotation = glm::vec3();
float       g_zoom = 1.0f;

#define DIM 1.0f
std::vector<drawVert> vertData =
{
	{ glm::vec3(DIM,DIM,    0),	   glm::vec3(0,0,1), glm::vec2(1,1) },
	{ glm::vec3(-DIM,DIM,  0),    glm::vec3(0,0,1), glm::vec2(0,1) },
	{ glm::vec3(-DIM, -DIM,  0),    glm::vec3(0,0,1), glm::vec2(0,0) },
	{ glm::vec3(DIM,-DIM,  0),    glm::vec3(0,0,1), glm::vec2(1,0) },
};

// Setup indices
std::vector<uint32_t> indexBuffer = { 0, 1, 2, 2,3,0 };

namespace Renderer
{
	void RenderPassMesh::UpdateUniformBufferData()
	{
		// Update matrices
		int g_iDesktopWidth = 800;
		int g_iDesktopHeight = 600;
		m_UboData.projectionMatrix = glm::perspective(glm::radians(60.0f), (float)g_iDesktopWidth / (float)g_iDesktopHeight, 0.1f, 256.0f);

		m_UboData.viewMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, g_zoom));

		m_UboData.modelMatrix = m_UboData.viewMatrix * glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -5.0f));
		m_UboData.modelMatrix = glm::rotate(m_UboData.modelMatrix, glm::radians(g_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		m_UboData.modelMatrix = glm::rotate(m_UboData.modelMatrix, glm::radians(g_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		m_UboData.modelMatrix = glm::rotate(m_UboData.modelMatrix, glm::radians(g_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		m_UboData.viewPos = glm::vec4(0.0f, 0.0f, -5.0f, 0.0f);
	}

	void RenderPassMesh::Init()
	{		
		LoadShaders("triangle.vert.spv", "triangle.frag.spv");
		CreatePipelineLayout("RenderPassMesh_PipelineLayout");
		CreateRenderPass("RenderPassMesh_RenderPass");
		CreateFramBuffer("RenderPassMesh_FrameBuffer");
		CrreateBufferLayout("RenderPassMesh_BufferLayout");
		CreatePipeline("RenderPassMesh_Pipeline");

		//SENDING DATA TO GPU
		struct Vertex
		{
			float pos[3];
			float uv[2];
			float normal[3];
		};


		uint32_t indexBufferSize = indexBuffer.size() * sizeof(uint32_t);

		//Update UBO
		UpdateUniformBufferData();

		m_StagingBufferVerticesRef = CreateBuffer("RenderPassMeshVertBuffer", VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertData.data(), static_cast<uint32_t>(vertData.size() + sizeof(drawVert)));
		m_StagingBufferIndicesRef = CreateBuffer("RenderPassMeshIndexBuffer", VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indexBuffer.data(), indexBufferSize);
		m_UniformBufferRef = CreateUniformBuffer("RenderPassMeshUniformBuffer", VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, &m_UboData, sizeof(m_UboData));

		m_DrawCallRef = CreateDrawCall("RenderPassMeshDrawCall", indexBuffer.size());
	}

	void RenderPassMesh::Destroy()
	{

	}

	void RenderPassMesh::Render(float dt, float width, float height)
	{
		VkClearValue clearValues[2];
		clearValues[0].color = { { 0.5f, 0.5f, 0.5f, 1.0f } };
		clearValues[1].depthStencil = { 1.0f, 0 };

		Renderer::Vulkan::RenderSystem::BeginRenderPass(m_RenderPassRef, m_FrameBufferRefs[Renderer::Vulkan::RenderSystem::backBufferIndex], VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS, 2, clearValues);
		Renderer::Vulkan::DrawCall::QueuDrawCall(m_DrawCallRef, m_FrameBufferRefs[Renderer::Vulkan::RenderSystem::backBufferIndex], m_RenderPassRef, width, height);
		Renderer::Vulkan::RenderSystem::EndRenderPass();
	}

	bool RenderPassMesh::LoadShaders(const std::string& vertShader, const std::string& fragShader)
	{
		//Create GPU Resource
		DOD::Ref vert_ref = Renderer::Resource::GpuProgramManager::CreateGPUProgram(vertShader);
		DOD::Ref frag_ref = Renderer::Resource::GpuProgramManager::CreateGPUProgram(fragShader);

		//Compile and set to created gpu resource reference
		bool bSaderLoaded = Renderer::Resource::GpuProgramManager::LoadAndCompileShader(vert_ref, "../../Assets/Shaders/", VK_SHADER_STAGE_VERTEX_BIT);
		bSaderLoaded |= Renderer::Resource::GpuProgramManager::LoadAndCompileShader(frag_ref, "../../Assets/Shaders/", VK_SHADER_STAGE_FRAGMENT_BIT);

		if (bSaderLoaded == false)
		{
			Renderer::Resource::GpuProgramManager::destroyResource(vert_ref);
			Renderer::Resource::GpuProgramManager::destroyResource(frag_ref);
			return false;
		}

		m_VertShaderRef = vert_ref;
		m_FragShaderRef = frag_ref;

		return true;
	}

	void RenderPassMesh::CreatePipelineLayout(const std::string& pipelineLayoutName)
	{
		std::vector<DOD::Ref> PipeleinLayoutRefs;

		//Set bindings positions
		m_PipeleinLayoutRef = Renderer::Resource::PipelineLayoutManager::CreatePipelineLayout(pipelineLayoutName);

		//Describe at which position uniform and image buffer object goes
		auto& descriptorSetLayout = Renderer::Resource::PipelineLayoutManager::GetDescriptorSetLayoutBinding(m_PipeleinLayoutRef);
		descriptorSetLayout.push_back(VkTools::Initializer::DescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0));

		PipeleinLayoutRefs.push_back(m_PipeleinLayoutRef);
		Renderer::Resource::PipelineLayoutManager::CreateResource(PipeleinLayoutRefs);
	}

	void RenderPassMesh::CreateRenderPass(const std::string& renderPassName)
	{
		std::vector<DOD::Ref> RenderPassRefs;

		m_RenderPassRef = Renderer::Resource::RenderPassManager::CreateRenderPass(renderPassName);
		Renderer::Resource::RenderPassManager::ResetToDefault(m_RenderPassRef);

		AttachementDescription sceneAttachment =
		{
			Renderer::Vulkan::RenderSystem::vkColorFormatToUse,
			AttachementFlags::kClearOnLoad,
			false
		};

		Renderer::Resource::RenderPassManager::GetAttachementDescription(m_RenderPassRef).push_back(sceneAttachment);

		RenderPassRefs.push_back(m_RenderPassRef);
		Renderer::Resource::RenderPassManager::CreateResource(RenderPassRefs);
	}

	void RenderPassMesh::CreateFramBuffer(const std::string& frameBufferName)
	{
		m_FrameBufferRefs.clear();
		m_FrameBufferRefs.reserve(Renderer::Vulkan::RenderSystem::vkSwapchainImages.size());

		for (int backBufferIndex = 0; backBufferIndex < Renderer::Vulkan::RenderSystem::vkSwapchainImages.size(); backBufferIndex++)
		{
			//Create image
			std::string imageName = frameBufferName + std::to_string(backBufferIndex) + "_Image";
			DOD::Ref imageRef = Renderer::Resource::ImageManager::CreateImage(imageName);
			Renderer::Resource::ImageManager::ResetToDefault(imageRef);
			Renderer::Resource::ImageManager::GetImageDimensions(imageRef) = glm::uvec3(800, 600, 1u);
			Renderer::Resource::ImageManager::GetImageFormat(imageRef) = Renderer::Vulkan::RenderSystem::vkColorFormatToUse;
			Renderer::Resource::ImageManager::CreateResource(imageRef);

			std::string frBufferName = frameBufferName + std::to_string(backBufferIndex);
			DOD::Ref frame_buffer_Ref = Renderer::Resource::FrameBufferManager::CreateFrameBuffer(frBufferName);
			Renderer::Resource::FrameBufferManager::ResetToDefault(frame_buffer_Ref);
			Renderer::Resource::FrameBufferManager::GetDimensions(frame_buffer_Ref) = Renderer::Vulkan::RenderSystem::backBufferDimensions;
			Renderer::Resource::FrameBufferManager::GetAttachedImiges(frame_buffer_Ref).push_back(imageRef);
			Renderer::Resource::FrameBufferManager::GetRenderPassRef(frame_buffer_Ref) = m_RenderPassRef;
			Renderer::Resource::FrameBufferManager::CreateResource(frame_buffer_Ref);

			m_FrameBufferRefs.push_back(frame_buffer_Ref);
		}
	}

	void RenderPassMesh::CrreateBufferLayout(const std::string& bufferLayoutName)
	{
		m_BufferLayoutRef = Renderer::Resource::BufferLayoutManager::CreateBufferLayout("BufferLayout1");
		auto& buffer_layout_description = Renderer::Resource::BufferLayoutManager::GetBufferLayoutDescription(m_BufferLayoutRef);

		//Describe layout of data that will be sent from cpu to gpu
		buffer_layout_description =
		{
			{0, Renderer::Resource::BufferObjectType::VERTEX, VK_FORMAT_R32G32B32_SFLOAT},
			{1, Renderer::Resource::BufferObjectType::COLOR, VK_FORMAT_R32G32B32_SFLOAT},
			{2, Renderer::Resource::BufferObjectType::TEX, VK_FORMAT_R32G32_SFLOAT}
		};

		Renderer::Resource::BufferLayoutManager::CreateResource(m_BufferLayoutRef);
	}

	void RenderPassMesh::CreatePipeline(const std::string& pipelineName)
	{
		std::vector<DOD::Ref> PipelineRefs;

		m_PipelineRef = Renderer::Resource::PipelineManager::CreatePipeline(pipelineName);
		Renderer::Resource::PipelineManager::GetVertexShader(m_PipelineRef)		= m_VertShaderRef;
		Renderer::Resource::PipelineManager::GetFragmentShader(m_PipelineRef)		= m_FragShaderRef;
		Renderer::Resource::PipelineManager::GetPipelineLayoutRef(m_PipelineRef)	= m_PipeleinLayoutRef;
		Renderer::Resource::PipelineManager::GetRenderPassRef(m_PipelineRef)		= m_RenderPassRef;
		Renderer::Resource::PipelineManager::GetbufferLayoutRef(m_PipelineRef)	= m_BufferLayoutRef;
		PipelineRefs.push_back(m_PipelineRef);

		Renderer::Resource::PipelineManager::CreateResource(PipelineRefs);
	}

	DOD::Ref RenderPassMesh::CreateBuffer(const std::string& name, VkBufferUsageFlagBits usage, void* bufferData, int32_t bufferSize)
	{
		DOD::Ref bufferRef = Renderer::Resource::BufferObjectManager::CreateBufferOjbect(name);

		Renderer::Resource::BufferObjectManager::GetBufferSize(bufferRef) = bufferSize;
		Renderer::Resource::BufferObjectManager::GetBufferUsageFlag(bufferRef) = usage;
		Renderer::Resource::BufferObjectManager::GetBufferData(bufferRef) = bufferData;
		Renderer::Resource::BufferObjectManager::CreateResource(bufferRef, Renderer::Vulkan::RenderSystem::vkPhysicalDeviceMemoryProperties);

		return bufferRef;
	}

	DOD::Ref RenderPassMesh::CreateUniformBuffer(const std::string& name, VkBufferUsageFlagBits usage, void* bufferData, int32_t bufferSize)
	{
		DOD::Ref uniformBufferRef = Renderer::Resource::UniformBufferManager::CreateUniformBufferOjbect(name);

		Renderer::Resource::UniformBufferManager::GetUniformBufferSize(uniformBufferRef) = bufferSize;
		Renderer::Resource::UniformBufferManager::GetUniformBufferUsageFlag(uniformBufferRef) = usage;
		Renderer::Resource::UniformBufferManager::GetUniformBufferData(uniformBufferRef) = bufferData;
		Renderer::Resource::UniformBufferManager::CreateResource(uniformBufferRef, Renderer::Vulkan::RenderSystem::vkPhysicalDeviceMemoryProperties);

		return uniformBufferRef;
	}

	DOD::Ref RenderPassMesh::CreateDrawCall(const std::string& name, int32_t indexBufferSize)
	{
		std::vector<DOD::Ref> drawCallsToCreate;
		const auto& drawCallRef = Renderer::Resource::DrawCallManager::CreateDrawCall(name);

		auto& binding_infos = Renderer::Resource::DrawCallManager::GetBindingInfo(drawCallRef);
		binding_infos.push_back(std::move(Renderer::Resource::BindingInfo{ 0, m_UniformBufferRef }));

		Renderer::Resource::DrawCallManager::GetIndexCount(drawCallRef) = indexBufferSize;
		Renderer::Resource::DrawCallManager::GetIndexBufferRef(drawCallRef) = m_StagingBufferIndicesRef;
		Renderer::Resource::DrawCallManager::GetVertexBufferRef(drawCallRef) = m_StagingBufferVerticesRef;
		Renderer::Resource::DrawCallManager::GetPipelineLayoutRef(drawCallRef) = m_PipeleinLayoutRef;
		Renderer::Resource::DrawCallManager::GetPipelineRef(drawCallRef) = m_PipelineRef;

		drawCallsToCreate.push_back(drawCallRef);
		Renderer::Resource::DrawCallManager::CreateResource(drawCallsToCreate);

		return drawCallRef;
	}

}
