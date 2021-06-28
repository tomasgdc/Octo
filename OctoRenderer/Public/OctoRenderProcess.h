#pragma once
#include "OctoRenderPassMesh.h"

namespace Renderer
{
	struct RenderProcess
	{
			static void Init();
			static void Update(float dt);
			static void Destroy();

		private:
			static std::vector<Renderer::RenderPassMesh> m_MeshRenderPasses;
	};
}