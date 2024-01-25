# RENDER API 2 #

## GLOBAL OBJECTS
```Python
class EspRenderContext:
  def init(EspWindow& window) -> None:
      # This function is responsible 
      # for initializing all resources 
      # related to the external render 
      # API, which are unchanged while 
      # the application is running.
  
  def terminate() -> None:
      # Freeing resources related to 
      # the external render API.
  
  @staticmethod
  def build(EspWindow& window) -> EspRenderContext:
      # A static function returning a
      # singleton of the EspRenderContext
      # class.
```

```Python
class EspDebugMessenger:
  def init() -> None:
      # This function is responsible 
      # for initializing the diagnostic 
      # message handler from the external 
      # render API, in debug mode.
  
  def terminate() -> None:
      # Freeing up resources.
  
  @staticmethod
  def create() -> EspDebugMessenger:
      # Creating a singleton of the 
      # EspDebugMessenger class.
```

```Python
class EspWorkOrchestrator:
  def init() -> None:
      # Initialization of external
      # render API resources related 
      # to the process of presenting
      # pipeline results on the screen.

  def terminate() -> None:
      # Freeing up resources.

  def begin_frame() -> None:
      # Start recording commands 
      # to the global command buffer.

  def end_frame() -> None:
      # End of recording commands
      # to the global command buffer
      # and sending them for execution.

  @staticmethod
  def get_swap_chain_extent() -> std::pair<uint32_t, uint32_t>:
      # Returns the width and height
      # of the surface on which we draw.

  @staticmethod
  def get_swap_chain_extent_aspect_ratio() -> float:
      # Returns the ratio of the width
      # to the height of the surface on
      # which we draw.
```

```Python
class EspJob:

  def init() -> None:
      # Initialization of external 
      # render API resources related 
      # to commands sent to the GPU.

  def terminate() -> None:
      # Freeing up resources.

  def done_all_jobs() -> None:
      # Pauses further program 
      # execution until the external 
      # render API command on the 
      # GPU finishes executing.

  @staticmethod
  def draw(uint32_t vertex_count) -> None:
      # Draw the given number
      # of vertices from the 
      # connected vertex buffer.

  @staticmethod
  def draw(uint32_t vertex_count, uint32_t instance_count) -> None:
      # Draw the given number of
      # vertices from the attached
      # vertex buffer, as many times
      # as the given `instance_count` instances.

  @staticmethod
  def draw_indexed(
              uint32_t index_count, 
              uint32_t instance_count = 1, 
              uint32_t first_index = 0) -> None:
      # This function draws the given
      # number of vertices, based on
      # the index buffer. We draw 
      # `instance_count` times this 
      # way. We start with the `first_index` index.

  @staticmethod
  def draw(EspCommandBufferId* id, uint32_t vertex_count) -> None:
      # Draw the given number
      # of vertices from the 
      # connected vertex buffer.
      # Save the command to the 
      # command buffer indicated 
      # as the `id` argument.
  
  @staticmethod
  def draw(
          EspCommandBufferId* id, 
          uint32_t vertex_count, 
          uint32_t instance_count) -> None:
      # Draw the given number of
      # vertices from the connected
      # vertex buffer. Save the command
      # to the command buffer indicated
      # as the `id` argument.

  @staticmethod
  def draw_indexed(
        EspCommandBufferId* id,
        uint32_t index_count,
        uint32_t instance_count = 1,
        uint32_t first_index  = 0) -> None:
      # Draw the given number of vertices
      # from the attached vertex buffer,
      # as many times as the given `instance_count`
      # instances. Save the command to the
      # command buffer indicated as the `id` argument.

  @staticmethod
  def copy_image(
        EspCommandBufferId* id,
        std::shared_ptr<EspTexture> src_texture,
        EspImageLayout src_layout,
        std::shared_ptr<EspTexture> dst_texture,
        EspImageLayout dst_layout,
        EspImageCopy region) -> None:
      # Copy an image from one texture to 
      # another. Save the command to the
      # command buffer indicated as 
      # the `id` argument.

  @staticmethod
  def transform_image_layout(
        EspCommandBufferId* id,
        std::shared_ptr<EspTexture> texture,
        EspImageLayout old_layout,
        EspImageLayout new_layout,
        EspImageSubresourceRange image_subresource_range) -> None:
      # Change the texture layout
      # from `old_layout` to `new_layout`.
      # Save the command to the command
      # buffer indicated as the `id` argument.

```
## LOCAL OBJECTS

### CONST OBJECTS

```Python
class EspRenderPlan:
  def add_block(std::shared_ptr<EspBlock> block) -> None:
      # Added EspBlock to the set
      # of all colour attachments.

  def add_depth_block(std::shared_ptr<EspDepthBlock> depth_block) -> None:
      # Set given EspDepthBlock as
      # depth attachment.

  def set_new_layout(EspImageLayout new_layout) -> None:
      # Set memory layout of
      # rendered texture.

  def set_command_buffer(EspCommandBufferId* id) -> None:
      # Set command buffer which will
      # record commands.

  def build() -> None:
      # Initialize/Build the internal 
      # structures necessary for 
      # image rendering.

  def begin_plan() -> None:
      # Start drawing to the 
      # specified attachments.

  def end_plan() -> None:
      # End drawing to the 
      # specified attachments.
  
  @staticmethod
  def create() -> std::unique_ptr<EspRenderPlan>:
      # Create an EspRenderPlan that 
      # does not draw on the screen, 
      # only to selected attachments.
      
  @staticmethod
  def create_final(
      glm::vec3 clear_color = { 0.1, 0.1, 0.1 },
      EspSampleCountFlag sample_count_flag = EspSampleCountFlag::ESP_SAMPLE_COUNT_1_BIT
      ) -> std::unique_ptr<EspRenderPlan>:
      # Create an EspRenderPlan 
      # that draws on the screen.
```

```Python
class EspBlock:
  def get_width() -> uint32_t:
      # Return the width of EspBlock

  def get_height() -> uint32_t:
      # Return the height of EspBlock

  def get_clear_color() -> glm::vec3:
      # Return the color of EspBlock

  def get_sample_count_flag() -> EspSampleCountFlag:
      # Return the sample count flag of EspBlock

  def use_as_texture() -> std::shared_ptr<EspTexture>:
      # This function allows you to use a 
      # block as a texture without turning 
      # it into a texture. This means that 
      # the EspBlock instance frees up
      # resources so it must be preserved.

  @staticmethod
  def build(
        EspBlockFormat format,
        EspSampleCountFlag sample_count_flag,
        glm::vec3 clear_color) -> std::shared_ptr<EspBlock>:
      # Create EspBlock

  @staticmethod
  def build(
        EspBlockFormat format,
        EspSampleCountFlag sample_count_flag,
        uint32_t width,
        uint32_t height,
        glm::vec3 clear_color) -> std::shared_ptr<EspBlock>:
      # Create EspBlock
  
  @staticmethod
  def extract_texture(std::shared_ptr<EspBlock> block) -> std::shared_ptr<EspTexture>:
      # Change EspBlock into EspTexture. 
      # This function transfers all resources 
      # from the EspBlock instance to the texture.
      # Therefore, releasing EspBlock
      # will not affect the created EspTexture.
```

```Python
class EspDepthBlock:
  def clear() -> None:
      # EspDepthBlock can in theory be used
      # across multiple EspRenderPlans, 
      # so you need to mark the moment 
      # when it should be cleared.

  def get_width() -> uint32_t:
      # Return the width of EspDepthBlock.

  def get_height() -> uint32_t:
      # Return the height of EspDepthBlock.

  def get_sample_count_flag() -> EspSampleCountFlag:
      # Return the sample count flag of EspDepthBlock.
  
  def get_image_usage_flag() -> EspImageUsageFlag:
      # Return the usage flag of the block.

  @staticmethod
  def build(
        EspDepthBlockFormat format,
        EspSampleCountFlag sample_count_flag,
        EspImageUsageFlag image_usage_flag = 
        EspImageUsageFlag::ESP_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
      ) -> std::unique_ptr<EspDepthBlock>:
      # Create EspDepthBlock.
  
  @staticmethod
  def build(
        EspDepthBlockFormat format,
        EspSampleCountFlag sample_count_flag,
        EspImageUsageFlag image_usage_flag,
        uint32_t width,
        uint32_t height
      ) -> std::unique_ptr<EspDepthBlock>:
      # Create EspDepthBlock.
```

```Python
class EspWorkerBuilder:
  def enable_depth_test(EspDepthBlockFormat format, EspCompareOp compare_op) -> None:
      # Enables the possibility of 
      # depth testing for a given pipeline.
      # If EspRenderPlan has EspDepthBlock,
      # this option must be used.

  def enable_multisampling(EspSampleCountFlag sample_count_flag) -> None:
      # Enables multisampling for a given
      # pipeline. If EspRenderPlan supports
      # more than one sample, this 
      # option must be used.

  def set_attachment_formats(std::vector<EspBlockFormat> formats) -> None:
      # If we want render plan to write
      # to more than one attachment, 
      # the formats of all attachments 
      # must be provided in this function.
      # This function overwrites the previous
      # setting and the basic setting,
      # which is the attachment of the screen format.

  def set_shaders(std::unique_ptr<SpirvResource> spirv_resource) -> None:
      # Set resources with shader code.

  def set_specialization(const SpecializationConstantMap& spec_const_map) -> None:
      # Set specialization constant for shaders' programs.

  def set_vertex_layouts(std::vector<EspVertexLayout> vertex_layouts) -> None:
      # Set the layout of vertex
      # attachments for the pipeline.

  def set_worker_layout(std::unique_ptr<EspUniformMetaData> uniforms_meta_data) -> None:
      # Set the layout of 
      # used uniforms by pipeline.

  def build_worker() -> std::unique_ptr<EspWorker>:
      # Create a pipeline based 
      # on the information provided
      # earlier and the code.

  @staticmethod
  def create() -> std::unique_ptr<EspWorkerBuilder> :
      # Create EspWorkerBuilder.
```

```Python
class EspWorker:
  def attach() -> None:
      # Attach the given pipeline 
      # to the global command buffer.

  def only_attach(EspCommandBufferId* id) -> None:
      # Attach the given pipeline 
      # to the command buffer and
      # set the viewport and scissors
      # according to the dimensions of
      # the application window.
  
  def set_viewport(EspViewport viewport) -> None:
      # Set viewport of the pipeline.

  def set_scissors(EspScissorRect scissor_rect) -> None:
      # Set scissors of the pipeline.

  def set_viewport(EspCommandBufferId* id, EspViewport viewport) -> None:
      # Set viewport of the pipeline using given command buffer.

  def set_scissors(EspCommandBufferId* id, EspScissorRect scissor_rect) -> None:
      # Set scissors of the pipeline using given command buffer.

  def create_uniform_manager(
        int start_managed_ds = -1,
        int end_managed_ds   = -1
        ) -> std::unique_ptr<EspUniformManager>:
      # Create a EspUniformManager that is
      # compatible with the given pipeline.
```

```Python
class EspUniformManager:
  def build() -> None:
      # Build EspUniformManager based
      # on the collected information.

  def attach() -> None:
      # Ettach EspUniformManager to 
      # the global command buffer

  def attach(EspCommandBufferId* id)  -> None:
      # Ettach EspUniformManager to 
      # the given command buffer by `id`.

  def load_block(
        uint32_t set, 
        uint32_t binding, 
        EspBlock* block) -> EspUniformManager&:
      # Load block as texture under given
      # set and binding.

  def load_texture(
        uint32_t set, 
        uint32_t binding, 
        std::shared_ptr<EspTexture> texture) -> EspUniformManager&:
      # Load texture under given
      # set and binding.
  
  def set_buffer_uniform(
        uint32_t set,
        uint32_t binding,
        uint64_t offset,
        uint32_t size,
        void* data) -> EspUniformManager&:
      # Set buffer uniform permanently.

  def update_buffer_uniform(
        uint32_t set,
        uint32_t binding,
        uint64_t offset,
        uint32_t size,
        void* data) -> EspUniformManager&:
      # Update the selected buffer
      # uniform with the given data.
  
  def update_push_uniform(
        uint32_t index, 
        void* data) -> EspUniformManager&:
      # Update the selected push
      # uniform with the given data.
      # Using global command buffer.

  def update_push_uniform(
        EspCommandBufferId* id, 
        uint32_t index, 
        void* data) -> EspUniformManager&:
      # Update the selected push
      # uniform with the given data.
      # Using selected command buffer.
```

```Python
class EspUniformMetaData:
  def  establish_descriptor_set() -> EspUniformMetaData&:
      # Create a new descriptor set and add to it all
      # following uniforms.

  def add_buffer_uniform(
        EspUniformShaderStage stage,
        uint32_t size_of_data_chunk,
        uint32_t count_of_data_chunks = 1) -> EspUniformMetaData&:
      # Create buffer uniform.

  def add_texture_uniform(
        EspUniformShaderStage stage,
        uint32_t count_of_textures = 1) -> EspUniformMetaData&:
      # Create texture uniform.

  def add_push_uniform(
      EspUniformShaderStage stage, 
      uint32_t offset, 
      uint32_t size) -> EspUniformMetaData&:
      # Create push uniform.
  
  @staticmethod
  def create() -> std::unique_ptr<EspUniformMetaData>:
      # Create EspUniformMetaData 
      # based on the collected data.
```

### TEMPORARY OBJECTS

```Python
class EspCommandBuffer:
  @staticmethod
  def begin_only_once() -> std::unique_ptr<EspCommandBufferId>:
      # Start recording commands 
      # to local command buffer.
      
  @staticmethod
  def end_only_once(
      std::unique_ptr<EspCommandBufferId> command_buffer_id
      ) -> None:
      # Stop recording commands
      # to given command buffer and
      # submit all commands.
```

### RESOURCES
```Python
class EspShader:

  def attach() -> None:
      # Attach shader to the global
      # command buffer, set viewport and scissors.

  def create_uniform_manager(
        int start_managed_ds = -1, 
        int end_managed_ds = -1
        ) -> std::unique_ptr<EspUniformManager>
      # Create EspUniformManager.

  def enable_depth_test(
        EspDepthBlockFormat format, 
        EspCompareOp compare_op
        )  -> None:
      # Enable depth testing.

  def enable_multisampling(
        EspSampleCountFlag sample_count_flag
        )  -> None:
      # Enable multisampling.

  def set_attachment_formats(
        std::vector<EspBlockFormat> formats
        )  -> None:
      # Set colour attachments.

  def set_vertex_layouts(
        std::vector<EspVertexLayout> vertex_layouts
        )  -> None:
      # Set vertex layouts.

  def set_worker_layout(
        std::unique_ptr<EspUniformMetaData> uniforms_meta_data
        )  -> None:
      # Set layout of uniforms.

  def only_attach(EspCommandBufferId* id)  -> None:
      # Only attach pipeline to the
      # given command buffer.

  def set_viewport(
        EspCommandBufferId* id, 
        EspViewport viewport
        )  -> None:
      # Set viewport under given command buffer.

  def set_scissors(
        EspCommandBufferId* id, 
        EspScissorRect scissor_rect
      )  -> None:
    # Set scissors under given command buffer.

  def build_worker()  -> None:
      # Build shader based on the collected data.

  @staticmethod
  def create(
        const std::string& name, 
        td::unique_ptr<SpirvResource> spirv_resource
        ) -> std::shared_ptr<EspShader>:
      # Create Shader.
```

```Python
class EspTexture:
  @staticmethod
  def create_raw_texture(EspRawTextureParams params) ->  std::shared_ptr<EspTexture>:
      # Create empty texture.
```

```Python
class EspVertexBuffer:
  def update(
        void* data, 
        uint32_t instance_size, 
        uint32_t instance_count, 
        uint64_t offset
        ) -> None:
      # ...

  def  get_vertex_count() -> uint32_t:
      # Get number of vertex.

  def attach() -> None:
      # Attach EspVertexBuffer to
      # the global command buffer.

  def attach_instanced(EspVertexBuffer& instance_buffer) -> None:
      # ....

  def attach(EspCommandBufferId* id) -> None:
      # Attach EspVertexBuffer to
      # the given command buffer.

  def attach_instanced(
        EspCommandBufferId* id, 
        EspVertexBuffer& instance_buffer
        ) -> None:
      # ...

  @staticmethod
  def create(
        void* data,
        uint32_t vertex_size,
        uint32_t vertex_count,
        BufferType type = LOCAL
        ) -> std::unique_ptr<EspVertexBuffer>:
      # Create EspVertexBuffer.
```

```Python
class EspIndexBuffer:
  def  get_index_count() -> uint32_t:
      # Get number of indices.

  def attach() -> None:
      # Attach EspIndexBuffer to the
      # global command buffer.

  def attach(EspCommandBufferId* id) -> None:
      # Attach EspIndexBuffer to the
      # given command buffer.

  @staticmethod
  def create(
        uint32_t* indices, 
        uint32_t index_count
        ) -> std::unique_ptr<EspIndexBuffer>:
      # Create EspIndexBuffer.
```
