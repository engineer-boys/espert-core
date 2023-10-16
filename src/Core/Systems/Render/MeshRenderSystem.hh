#ifndef RENDER_MESH_RENDER_SYSTEM_H_
#define RENDER_MESH_RENDER_SYSTEM_H_

#include "EspRenderSystem.hh"

namespace esp
{
	class MeshRenderSystem : public EspRenderSystem
	{
	 public:
		MeshRenderSystem(EspRenderContext& render_context);

		MeshRenderSystem(const MeshRenderSystem&) = delete;
		MeshRenderSystem& operator=(const MeshRenderSystem&) = delete;

		void render(EspRenderContext& context) override;
	};
}

#endif //RENDER_MESH_RENDER_SYSTEM_H_
