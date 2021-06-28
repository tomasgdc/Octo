CMAKE_MINIMUM_REQUIRED(VERSION 3.4)

PROJECT(OctoRenderer)

ADD_COMPILE_DEFINITIONS(VK_USE_PLATFORM_WIN32_KHR)

SET(HEADERS_RENDERER
	"Public/OctoRenderProcess.h"
	"Public/OctoRenderPassFullScreen.h"
	"Public/OctoRenderPassMesh.h"
)
SET(SOURCES_RENDERER
	"Private/OctoRenderProcess.cpp"
	"Private/OctoRenderPassFullScreen.cpp"
	"Private/OctoRenderPassMesh.cpp"
)
SOURCE_GROUP("Public"  FILES 	${HEADERS_RENDERER})
SOURCE_GROUP("Private" FILES 	${SOURCES_RENDERER})

SET(HEADERS_IMGUI
	"Public/ImGui/imconfig.h"
	"Public/ImGui/imgui.h"
	"Public/ImGui/imgui_internal.h"
	"Public/ImGui/stb_rect_pack.h"
	"Public/ImGui/stb_textedit.h"
	"Public/ImGui/stb_truetype.h"

)

SET(SOURCES_IMGUI
	"Private/ImGui/imgui.cpp"
	"Private/ImGui/imgui_demo.cpp"
	"Private/ImGui/imgui_draw.cpp"
)
SOURCE_GROUP("Public\\ImGui" FILES 	${HEADERS_IMGUI})
SOURCE_GROUP("Private\\ImGui" FILES ${SOURCES_IMGUI})


SET(HEADERS_FREETYPE
	"Public/FreeType/VkFont.h"
	"Public/FreeType/FreetypeLoad.h"

)
SET(SOURCES_FREETYPE
	"Private/FreeType/VkFont.cpp"
	"Private/FreeType/FreetypeLoad.cpp"
)
SOURCE_GROUP("Public\\FreeType" FILES 	${HEADERS_FREETYPE})
SOURCE_GROUP("Private\\FreeType" FILES 	${SOURCES_FREETYPE})

SET(HEADERS_VULKAN
	"Public/Vulkan/DrawCallManager.h"
	"Public/Vulkan/VkBufferLayoutManager.h"
	"Public/Vulkan/VkBufferObjectManager.h"
	"Public/Vulkan/VkUniformBufferManager.h"
	"Public/Vulkan/VkBufferObject.h"
	"Public/Vulkan/VulkanTextureLoader.h"
	"Public/Vulkan/VulkanDebug.h"
	"Public/Vulkan/VulkanAndroid.h"
	"Public/Vulkan/VulkanSwapChain.h"
	"Public/Vulkan/VulkanTools.h"
	"Public/Vulkan/VkGpuProgram.h"
	"Public/Vulkan/VkPipelineLayoutManager.h"
	"Public/Vulkan/VkRenderPassManager.h"
	"Public/Vulkan/VkPipelineManager.h"
	"Public/Vulkan/VkDrawCallDispatcher.h"
	"Public/Vulkan/VkRenderSystem.h"
	"Public/Vulkan/VkFrameBufferManager.h"
	"Public/Vulkan/VkImageManager.h"
	"Public/Vulkan/VkEnums.h"
	"Public/Vulkan/VkGPUMemoryManager.h"
	"Public/Vulkan/VulkanRendererInitializer.h"
)
SET(SOURCES_VULKAN
	"Private/Vulkan/DrawCallManager.cpp"
	"Private/Vulkan/VkBufferLayoutManager.cpp"
	"Private/Vulkan/VkBufferObjectManager.cpp"
	"Private/Vulkan/VkBufferObject.cpp"
	"Private/Vulkan/VkUniformBufferManager.cpp"
	"Private/Vulkan/VulkanTextureLoader.cpp"
	"Private/Vulkan/VulkanDebug.cpp"
	"Private/Vulkan/VulkanAndroid.cpp"
	"Private/Vulkan/VulkanTools.cpp"
	"Private/Vulkan/VulkanSwapChain.cpp"
	"Private/Vulkan/VkGpuProgram.cpp"
	"Private/Vulkan/VkPipelineLayoutManager.cpp"
	"Private/Vulkan/VkRenderPassManager.cpp"
	"Private/Vulkan/VkPipelineManager.cpp"
	"Private/Vulkan/VkDrawCallDispatcher.cpp"
	"Private/Vulkan/VkRenderSystem.cpp"
	"Private/Vulkan/VkFrameBufferManager.cpp"
	"Private/Vulkan/VkImageManager.cpp"
	"Private/Vulkan/VkGPUMemoryManager.cpp"
	"Private/Vulkan/VulkanRendererInitializer.cpp"
)

SOURCE_GROUP("Public\\Vulkan" FILES ${HEADERS_VULKAN})
SOURCE_GROUP("Private\\Vulkan" FILES ${SOURCES_VULKAN})

SET(HEADERS_GEOEMTRY
	"Public/Geometry/VertData.h"
	"Public/Geometry/JointTransform.h"
	"Public/Geometry/TangentAndBinormalCalculator.hpp"
)
SET(SOURCES_GEOEMTRY
	"Private/Geometry/JointTransform.cpp"
)

SOURCE_GROUP("Public\\Vulkan" FILES ${HEADERS_VULKAN})
SOURCE_GROUP("Private\\Vulkan" FILES ${SOURCES_VULKAN})

	
ADD_LIBRARY(${PROJECT_NAME} STATIC
	${HEADERS_RENDERER}
	${SOURCES_RENDERER}
	${HEADERS_IMGUI}	
	${SOURCES_IMGUI}
	${HEADERS_FREETYPE}
	${SOURCES_FREETYPE}
	${HEADERS_VULKAN}
	${SOURCES_VULKAN}
	${HEADERS_GEOEMTRY}
	${SOURCES_GEOEMTRY}
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC
	${VULKAN_LIB}
	${FREETYPE_LIBRARY}
	debug ${FREETYPE_LIBRARY_DEBUG}
	debug ${ASSIMP_LIBRARY_DEBUG}
	debug ${ZLIB_LIBRARY_DEBUG}
	optimized ${FREETYPE_LIBRARY_RELEASE}
	optimized ${ASSIMP_LIBRARY_RELEASE}
	optimized ${ZLIB_LIBRARY_RELEASE}
	)



TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC
	"Public"
	"Public/ImGui"
	"Public/FreeType"
	"Public/Geometry"
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}
	)