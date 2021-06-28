
//Renderer
#include <OctoRenderer/Public/Vulkan/VkEnums.h>
#include <OctoRenderer/Public/Vulkan/VkRenderSystem.h>
#include <OctoRenderer/Public/Vulkan/VkPipelineManager.h>
#include <OctoRenderer/Public/Vulkan/VkRenderPassManager.h>
#include <OctoRenderer/Public/Vulkan/VkPipelineLayoutManager.h>
#include <OctoRenderer/Public/Vulkan/VkGpuProgram.h>
#include <OctoRenderer/Public/Vulkan/VkBufferLayoutManager.h>
#include <OctoRenderer/Public/Vulkan/VkBufferObjectManager.h>
#include <OctoRenderer/Public/Vulkan/VkUniformBufferManager.h>
#include <OctoRenderer/Public/Geometry/VertData.h>

#include <OctoRenderer/Public/Vulkan/VkBufferObject.h>
#include <OctoRenderer/Public/Vulkan/DrawCallManager.h>
#include <OctoRenderer/Public/Vulkan/VkDrawCallDispatcher.h>

#include <OctoRenderer/Public/Vulkan/VkGPUMemoryManager.h>
#include <OctoRenderer/Public/Vulkan/VkImageManager.h>
#include <OctoRenderer/Public/Vulkan/VkFrameBufferManager.h>

#include <OctoRenderer/Public/Vulkan/VulkanRendererInitializer.h>
#include <OctoRenderer/Public/OctoRenderProcess.h>

//Core
#include <OctoCore/Public/DOD.h>

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

//Other
#include <filesystem>

//func
LRESULT CALLBACK HandleWindowMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
bool GenerateEvents(MSG& msg);

int g_iDesktopWidth = 800;
int g_iDesktopHeight = 600;

bool GenerateEvents(MSG& msg)
{
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return false;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return true;
}

int main(int argc, char *argv[])
{
	std::unique_ptr<VulkanRendererInitializer> renderer_initializer = std::make_unique<VulkanRendererInitializer>();
	renderer_initializer->CreateWindows(g_iDesktopWidth, g_iDesktopHeight, HandleWindowMessages);

	Renderer::Vulkan::RenderSystem::Init(true, true, "Triangle", renderer_initializer->m_hinstance, renderer_initializer->m_HwndWindows);
	Renderer::RenderProcess::Init();

#if defined(_WIN32)
	MSG msg;
#endif

	float delta = 0.0f;
	while (TRUE)
	{
		if (!GenerateEvents(msg))break;

		auto tStart = std::chrono::high_resolution_clock::now();
		Renderer::RenderProcess::Update(delta);
		auto tEnd = std::chrono::high_resolution_clock::now();
	}

	Renderer::Vulkan::RenderSystem::Shutdown();
	system("pause");

	return 0;
}

void WIN_Sizing(WORD side, RECT *rect)
{
	// restrict to a standard aspect ratio
	g_iDesktopWidth = rect->right - rect->left;
	g_iDesktopHeight = rect->bottom - rect->top;

	// Adjust width/height for window decoration
	RECT decoRect = { 0, 0, 0, 0 };
	AdjustWindowRect(&decoRect, WINDOW_STYLE | WS_SYSMENU, FALSE);
	uint32_t decoWidth = decoRect.right - decoRect.left;
	uint32_t decoHeight = decoRect.bottom - decoRect.top;

	g_iDesktopWidth -= decoWidth;
	g_iDesktopHeight -= decoHeight;

	Renderer::Vulkan::RenderSystem::backBufferDimensions = glm::uvec2(g_iDesktopWidth, g_iDesktopHeight);
}

LRESULT CALLBACK HandleWindowMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		//prepared = false;
		DestroyWindow(hWnd);
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		RECT rect;
		if (GetWindowRect(hWnd, &rect))
		{
			WIN_Sizing(wParam, &rect);
		}
		break;
	case WM_EXITSIZEMOVE:
		Renderer::Vulkan::RenderSystem::ResizeSwapchain();
		//RecreateAfterSwapchainResize();
		//g_Renderer.VWindowResize(g_iDesktopHeight, g_iDesktopWidth);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}