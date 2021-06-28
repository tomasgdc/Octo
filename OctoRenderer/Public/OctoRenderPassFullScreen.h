#pragma once
#include "OctoCore/Public/DODResource.h"

namespace Renderer
{
	struct RenderPassFullScreen
	{
			void Init();
			void Destroy();
			void Render(float dt);

		protected:
			bool LoadShaders(const std::string& vertShader, const std::string& fragShader);

		private:
			DOD::Ref m_VertShaderRef;
			DOD::Ref m_FragShaderRef;
	};
}