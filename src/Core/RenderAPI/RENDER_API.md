# MINIMAL RENDER API

```Python
class EspRenderContext():
    def init() -> None:
        # Init the render context.
    
    def terminate() -> None:
        # Terminate the render context.
```

```Python
class EspFrameManager:
    def __init__(clear_color=(0,0,0)):
        # This variables has to be
        # available as static members:
        self.frame_counter
        self.swapChainFramebuffers
        self.render_pass

        # Important members:
        self.commandBuffers
        self.clear_color

    def counter_update() -> None:
        # Update frame counter for
        # rendering in flight.
    
    def clear_color_update(r, g, b) -> None:
        # Set clear color
    
    def begin_frame() -> None:
        # Bind appropriate commandBuffer.
        # Begin appropriate commandBuffer.
        # Begin appropriate renderPass.
    
    def end_frame() -> None:
        # End appropriate renderPass.
        # End appropriate commandBuffer.
    
    def view_update() -> None:
        # Update viewport and scissor
        # by set new viewport and scissor values
        # for binded pipeline.
    
    def get_swap_chain_extent() -> std::pair<uint32,uint32>:
        # This will return width and height.
```

```Python
class EspPipelineBuilder:
    def set_shader(path_vertex, path_fragment) -> None:
        # Set given shaders as a pipeline shader program.

    def set_vertex_layout( vertex_vectors: list = {
            {
                .size: int
                .binding: int
                .attr: list = {
                    {
                        .location: int
                        .format: EspFormatEnum
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

    def set_pipeline_layout(pipeline_layout) -> None:
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
    
    def update_buffer_uniform(idx: int, size: int, data: void*) -> None:
        # Update buffer uniform with given index.
```

```Python
class EspIndexBuffer():
    def __init__(indices):
        # The constructor get a vector of indices and
        # allocate needed buffers.
    
    def size() -> int:
        # Return the number of indices.

    def attach():
        # Bind index buffer to pipeline.
```

```Python
class EspVertexBuffers():
    def add(vertices):
        # This function get a vector of vertices and
        # allocate needed buffor for it. All added buffors are
        # stored in this object.
    
    def attach():
        # Bind all buffers to pipeline.
```

```Python
class PipelineLayoutManufacture():
    def add_buffer_uniform(size: int) -> None:
        # This class create buffer uniforms without knowledge of user
        # to be coherent with render in flight. The first add_buffer_uniform
        # give this uniform an index 0 and so on.
    
    def add_push_constant_uniform() -> None:
        # I don't know this.

    def add_texture() -> None:
        # In the immediate future.
```

```Python
class EspCommands:
    @staticmethod
    def draw_indices(size_of_indices: int) -> None:
        # Draw using index buffer.

    @staticmethod
    def draw_vertices(size_of_vertices: int) -> None:
        # Draw using vertex buffer.
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
