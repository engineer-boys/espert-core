#include "MeshRenderSystem.hh"

namespace esp
{
	MeshRenderSystem::MeshRenderSystem(EspRenderContext& render_context)
		: EspRenderSystem(render_context.m_device)
	{
		/*PipelineConfigInfo pipeline_config{};
		EspPipeline::default_pipeline_config_info(pipeline_config);

		EspPipeline::Builder builder{render_context.m_device};
		builder
			//TODO: add shaders
			.set_vert_shader_path("")
			.set_frag_shader_path("");

		m_pipeline_layout = builder.build_pipeline_layout(pipeline_config);
		m_pipeline = builder.build_pipeline(pipeline_config, render_context.m_render_pass);*/
	}

	void MeshRenderSystem::render(EspRenderContext& context)
	{
		//m_pipeline->bind(context.m_command_buffer);
		//TODO: bind descriptor sets

		//...//
	}
}