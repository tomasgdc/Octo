#pragma once
#include "OctoCore/Public/DODResource.h"

//Vulkan
#include <ThirdParty/vulkan/vulkan.h>

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

namespace Renderer
{
	struct RenderPassMesh
	{
			void Init();
			void Destroy();
			void Render(float dt, float width, float height);

		protected:
			bool LoadShaders(const std::string& vertShader, const std::string& fragShader);
			void CreatePipelineLayout(const std::string& pipelineLayoutName);
			void CreateRenderPass(const std::string& renderPassName);
			void CreateFramBuffer(const std::string& frameBufferName);
			void CrreateBufferLayout(const std::string& bufferLayoutName);
			void CreatePipeline(const std::string& pipelineName);
			void UpdateUniformBufferData();

			DOD::Ref CreateBuffer(const std::string& name, VkBufferUsageFlagBits usage, void* bufferData, int32_t bufferSize);
			DOD::Ref CreateUniformBuffer(const std::string& name, VkBufferUsageFlagBits usage, void* bufferData, int32_t bufferSize);
			DOD::Ref CreateDrawCall(const std::string& name, int32_t indexBufferSize);

		private:

			struct UBO
			{
				glm::mat4 projectionMatrix;
				glm::mat4 modelMatrix;
				glm::mat4 viewMatrix;
				glm::vec4 viewPos;
				float lodBias = 0.0f;
			};

			UBO m_UboData;

			DOD::Ref m_VertShaderRef;
			DOD::Ref m_FragShaderRef;
			DOD::Ref m_PipeleinLayoutRef;
			DOD::Ref m_RenderPassRef;
			std::vector<DOD::Ref> m_FrameBufferRefs;
			DOD::Ref m_BufferLayoutRef;
			DOD::Ref m_PipelineRef;
			DOD::Ref m_DrawCallRef;

			//Data
			DOD::Ref m_StagingBufferVerticesRef;
			DOD::Ref m_StagingBufferIndicesRef;
			DOD::Ref m_UniformBufferRef;
	};
}