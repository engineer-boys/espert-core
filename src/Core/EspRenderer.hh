#ifndef RENDERER_RENDERER_H_
#define RENDERER_RENDERER_H_

#include "RenderAPI/EspRenderScheduler.hh"
#include "Scene/Components/Components.hh"
#include "Systems/Render/EspRenderSystem.hh"

namespace esp
{
	class EspRenderer
	{
	 private:
		//TODO: create scene
		// Scene _m_scene{};
		//TODO: scene - unordered_map[key: bit mask of components, value: list<entities>]
		//TODO: add LightRenderSystem - render system responsible only for adding light to meshes
		static EspRenderer* s_instance;
		static std::unordered_map<TagComponent::TAG, EspRenderSystem&> s_render_systems;

		EspDevice m_device;
		EspRenderContext m_context;

		std::unique_ptr<EspRenderScheduler> m_render_scheduler;

		EspRenderer(EspWindow& window);

	 public:
		~EspRenderer();

		static std::unique_ptr<EspRenderer> create(EspWindow& window);
		static void add_render_system(const TagComponent::TAG& tag, EspRenderSystem&& system);

		void begin_scene();
		void end_scene();

		inline static EspRenderer* get_instance() { return s_instance; }
		inline static EspRenderContext& get_render_context() { return s_instance->m_context; }

		void on_window_resized();
	};
}

#endif //RENDERER_RENDERER_H_
