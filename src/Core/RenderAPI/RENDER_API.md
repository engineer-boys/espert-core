# RENDER API

```Python
class EspRenderContext:
    def create_and_init(window: EspWindow) -> None:
    # Init render context.

    def terminate() -> None:
    # Terminate render context.
```

```Python
class EspFrameManager:
    def create_and_init(
            window: EspWindow,
            clear_color: glm::vec4(0.1, 0.1, 0.1, 1.0)) -> None:
        # Init frame manager.

        # Class members:
        self.window_width
        self.window_height
        self.depth_stencil
        self.clear_color

    def begin_frame() -> None:
        # Set clear values
        # Set viewport & scissors
        # Bind appropriate commandBuffer.
        # Begin appropriate commandBuffer.
        # Begin appropriate renderPass.

    def end_frame() -> None:
        # End appropriate renderPass.
        # End appropriate commandBuffer.

    def termiate() -> None:
        # Terminate frame manager

    def on_window_resized(e: WindowResizedEvent) -> None:
        # Recreate swap chain

    def get_swap_chain_extent() -> std::pair<uint32,uint32>:
        # Get swap chain (window) width and height.
    
    def set_depth_stencil(depth: float,
                          stencil: uint32_t) -> None:
        # Set both depth and stencil values
        # for render pass
    
    def set_clear_color(color: glm::vec3) -> None:
        # Set clear color r,g,b
    
    def set_clear_color(color: glm::vec4) -> None:
        # Set clear color r,g,b,a
```

```Python
class EspPipelineBuilder:
    def set_shaders(path_vertex, path_fragment) -> None:
        # Set given shaders as a pipeline shader program.
    
    def set_vertex_shader(path_vertex) -> None:
        # Set given shader
    
    def set_fragment_shader(path_fragment) -> None:
        # Set given shader

    def set_vertex_layouts( vertex_layouts: list = {
            {
                .size: int
                .binding: int
                .attrs: list = {
                    {
                        .location: int
                        .format: EspAttrFormat
                        .offset: int # This may be automatised in the future
                    },
                    {
                        # next attr
                    }
                    # etc ...
                }
            },
            {
                # next vertex vector
            }
            # etc ...
        }  ) -> None:
        # Set structure info about vertices in vertex buffers
        # which can be attach. This will be called vertex layout.

    def set_pipeline_layout(pipeline_layout: EspUniformMetaData) -> None:
        # Set layout for this pipeline. The layout describes
        # what data can be eaten by pipeline. The parameter is
        # PipelineLayoutManufacture which contains info about
        # needed uniforms, textures and it provides private API
        # for EspPipelineBuilder to get this data.
    
    def build_pipeline() -> EspPipeline:
        # Build EspPipeline object based on
        # given data from above functions.
```

```Python
class EspPipeline:
    def __init__():
        # This is private function. Because pipeline objects
        # can only be created by EspPipelineBuilder.
        # Inner members:
        self.buf_uniforms: map[int, list[void*]]
    
    def attach() -> None:
        # Bind this pipeline to using command buffer.
    
    def update_buffer_uniform(set: int, binding: int, size: int, data: void*) -> None:
        # Update buffer uniform with given index.
    
    def update_buffer_uniform(set: int, binding: int, elem: int, size: int, data: void*) -> None:
        # Update buffer uniform with given index.
    
    def attach_uniforms() -> None:
        # Attach uniforms to the current command buffer.
```

```Python
class EspIndexBuffer:
    def create(indices, index_count):
        # The constructor get a vector of indices and
        # allocate needed buffers.
    
    def get_index_count() -> int:
        # Return the number of indices.

    def attach():
        # Bind index buffer to pipeline.
```

```Python
class EspVertexBuffers:
    def add(data, vertex_size, vertex_count):
        # This function get a vector of vertices and
        # allocate needed buffor for it. All added buffors are
        # stored in this object.
    
    def attach():
        # Bind all buffers to pipeline.
```

```Python
class EspUniformMetaData:
    def __init__():
        self.current_ds_counter = -1

    def establish_descriptor_set() -> self:
        sefl.current_ds_counter += 1
    
    def add_buffer_uniform(stage: EspUniformShaderStage, size_of_data_chunk: int, count_of_data_chunks: int = 1) -> self:
        # Add buffer uniform to currently established ds.
        # If the count_of_data_chunks is greater than 1
        # the uniform is an array.
    
    def add_texture_uniform(count_of_textures: int = 1) -> None:
        # Add texture uniform to currently established ds.
```

```Python
class EspCommandHandler:
    def create_and_init() -> None:
        # Init command handler.

    def terminate() -> None:
        # Terminate command handler.
    
    @staticmethod
    def draw(vertex_count: uint32_t) -> None:
        # Draw using vertex buffer.

    @staticmethod
    def draw(vertex_count: uint32_t, 
             instance_count: uint32_t) -> None:
        # Draw with instancing using vertex buffer - 
        # vertex buffer for instances must be provided
        # and specified in pipeline creation.
    
    @staticmethod
    def draw_indexed(index_count: uint32_t) -> None:
        # Draw using index buffer.

    @staticmethod
    def draw_indexed(index_count: uint32_t,
             instance_count: uint32_t) -> None:
    # Draw with instancing using index buffer - 
    # vertex buffer for instances must be provided
    # and specified in pipeline creation.
```

# USAGE
```Python
class MVP:
    mode: mat4
    view: mat4
    proj: mat4

def main() -> None:
    renderContext = EspRenderContext()
    renderContext.init()

    # -------- WORKING USING CONTEXT ---------

    frameManager = EspFrameManger(Color(122,122,122))


    # 3 * float (for position), 3 * float (for color)
    cube_vertices: list[float] = get_cube_v_function()
    v_buffer = EspVertexBuffers()
    v_buffer.add(cube_vertices)
    
    cube_indices: list[int] = get_cube_i_function()
    i_buffer = EspIndexBuffer(cube_indices)

    pp_layout = PipelineLayoutManufacture()
    pp_layout.add_buffer_uniform(len(MVP))

    builder = EspPipelineBuilder()
    builder.set_shader(
        path_to_vertex_sh,
        path_to_fragment_sh,
    )
    builder.set_vertex_layout(
        [
            {
                .size = len(MVP)
                .binding = 0
                .attr = [
                    {
                        .location = 0
                        .format: EspFormatEnum.32_32_32_SFLOAT
                        .offset: # not important
                    },
                    {
                        .location = 1
                        .format: EspFormatEnum.32_32_32_SFLOAT
                        .offset: # not important
                    },
                ]
            }
        ]
    )
    builder.set_pipeline_layout(pp_layout)
    pp_graphic = builder.build_pipeline()

    while not end_of_program():
        frameManager.counter_update()
        frameManager.begin_frame()

            # ---- IN RENDER PASS -----
            pp_graphic.attach()
            frameManager.view_update()

            i_buffer.attach()
            v_buffer.attach()

            pp_graphic.update_buffer_uniform(
                idx = 0,
                size = len(MVP),
                data = update_mvp(),
            )

            EspCommands.draw_indices(i_buffer.size())
            # ---- -------------- -----

        framgeManager.end_frame()

    # -------- --------------------- ---------
    renderContext.terminate()
```
