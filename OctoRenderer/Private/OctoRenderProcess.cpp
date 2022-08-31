#include "OctoRenderProcess.h"
#include "Vulkan/VkRenderSystem.h"

namespace Renderer
{ 
	std::vector<Renderer::RenderPassMesh> RenderProcess::m_MeshRenderPasses;
	Renderer::RenderPassFullScreen RenderProcess::m_RenderPassFullScreen;

	void RenderProcess::Init()
	{
		Renderer::RenderPassMesh meshRenderPass;
		meshRenderPass.Init();
		m_MeshRenderPasses.emplace_back(std::move(meshRenderPass));

		m_RenderPassFullScreen.Init();
	}

	void RenderProcess::Update(float dt)
	{
		VkClearValue clearValues[2];
		clearValues[0].color = { { 0.5f, 0.5f, 0.5f, 1.0f } };
		clearValues[1].depthStencil = { 1.0f, 0 };

		Renderer::Vulkan::RenderSystem::StartFrame();

		m_RenderPassFullScreen.Render(dt, 800, 600);

		for (auto& meshRenderPass : m_MeshRenderPasses)
		{
			meshRenderPass.Render(dt, 800, 600);
		}

		Renderer::Vulkan::RenderSystem::EndFrame();
	}

	void RenderProcess::Destroy()
	{
		for (auto& meshRenderPass : m_MeshRenderPasses)
		{
			meshRenderPass.Destroy();
		}

		m_RenderPassFullScreen.Destroy();
	}
}