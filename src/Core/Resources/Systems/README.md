# Resource system

The resource system is responsible for handling various engine resources. It's primary purpose is to deliver a simple API to load and unload certain resources. Resource system class is a singleton and should be initialized at app start and terminated at app's exit.
```
class ResourceSystem:
    @staticmethod
    def create(asset_base_path: Path) -> ResourceSystem*:
        # initialize resource system and return it's instance

    def termiante() -> None:
        # termiantes the system

    @staticmethod
    def get_instance() -> ResourceSystem*:
        # return resource system instance

    @staticmethod
    def get_asset_base_path() -> Path:
        # return asset's location

    @staticmethod
    def register_laoder<type: ResourceType>(loader: Loader*) -> None:
        # register loader for specific resource type
        # the resource has to inherit after abstract Resource and loader 
        # after abstract Loader class

    @staticmethod
    def load<type: ResourceType>(path: Path, params: ResourceParams) -> Resource*:
        # runs load method of loader registered to supplied resource type

    @staticmethos
    def unload(Resource*) -> None:
        # runs unload method of loader registered to supplied resource type
```
Abstract resource class:
```
class Resource:
    def get_path() -> Path:
        # return full resource path

    def get_filename() -> str:
        # return resource filename
```
Abstract loader class:
```
class Loader:
    def load(path: Path, params: ResourceParams) -> Resource*:
        # load certain type of resource from disk
        # params can be used to affect loading process (eg flip image)
        # path should be relative to base asset path

    def unload(resource: Resource*) -> None:
        # destroys passed reosurce and releases it's memory
```

Avaliable predefined resource types:
- Resource - abstract
- BinaryResource - regular binary data
- TextResource - text data
- ImageReosurce - image data loaded with stb_image
- SpirvData - shader source data in spir-v format. The shader name should be relative path to the shader file without any extensions. The system will look for shader files with extensions .vert.spv, .frag.spv, .tesc.spv, .tese.spv, .comp.spv, geom.spv.

## Usage - loading predefined resource

```
def main() -> None:
    resource_system = ResourceSystem::create(asset_path)

    params = TextResourceParams()

    resource = ResourceSystem::load<TextResource>("test.txt", params)

    text_resource = (TextResource)resource

    ...

    ResourceSystem::unload(std::move(text_resource))
```

## Usage - loading user-defined resource
```
class TestResource(Resource):
    def __init__(path: Path, data_size: int, data: resource_data_t):
        super().__init__(path, size, data)

class TestResourceParams(ResourceParams):
    pass

class TestLoader(Loader):
    def load(path: Path, params: ResourceParams) -> Resource*:
        full_path = ResourceSystem::get_asset_base_path() / path

        data = 'test'

        return TestResource(full_path, len(data), resource_data_t(data, VOID_DELETER))


    def unload(resource: Resource) -> None:
        reosurce.reset(None)


def main() -> None:
    resource_system = ResourceSystem::create(asset_path)

    ResourceSystem::register_loader<TestResource>(TestLoader())

    params = TestResourceParams()
    resource = ResourceSystem::load<TestResource>("Test", params);

    test_resource = (TestResource)resource

    ...

    ResourceSystem::unload(std::move(resource))
``` 

# Texture system

The texture system is responsible for handling textures. It conserves time and memory by caching textures and returning their references. It uses resoruce system to load images. It generates some default textures which are used in case a specific texture couldn't be loaded. Texture system class is a singleton and should be initialized at app start and terminated at app's exit.

```
class TextureSystem:
    @staticmethod
    def create() -> TextureSystem*:
        # initializes and returns texture system instance

    def termiante() -> None:
        # termiantes the system

    @staticmethod
    def acquire(name: string) -> Texture:
        # returns texture reference and loads it if necessary
        # name of texture has to be the same as the relative path 
        # to its asset file

    @staticmethod
    def release(name: string) -> None:
        # destroys texture resource and removes it from reference map

    @staticmethod
    def get_default_texture(type: EspTextureType) -> Texture:
        # returns default texture of <type>
```

```
class Texture:
    def __init__(name: str, data: int[], channel_count: int, width: int, height: int, mip_levels: int):

    def __init__(name: str, image: ImageResource):

    def get_name() -> str:
        # return texture name

    def get_size() -> int:
        # return texture data size

    def get_channel_count() -> int:
        # return number of texture channels

    def get_width() -> int:
        # return texture width

    def get_height() -> int:
        # return texture height

    def get_pixels() -> int[]:
        # return texture data 

    def has_transparency -> bool:
        # returns True if texture has 4 channels and at least one pixel
        # with alpha less than 255
```

## Usage
```
def main() -> None:
    resource_system = ResourceSystem::create(asset_path)
    texture_system  = TextureSystem::create()

    texture = TextureSystem::acquire("test.jpg")

    ...

    TextureSystem::release("test.jpg")
```

# Shader system

The shader system is responsible for loading and handling shaders. It conserves time and memory by caching shaders and returning their references. It uses resoruce system to load spir-v shader soruces and then generates EspPipeline with it. It loads a default shader with name 'default' which os used in case a specific shader couldn't be loaded. For now it extends functionality of EspPipeline which will be changed with automatic pipeline generation. Shader system class is a singleton and should be initialized at app start and terminated at app's exit.

```
class ShaderSystem:
    @staticmethod
    def create() -> ShaderSystem*:
        # initializes and returns shader system instance

    def termiante() -> None:
        # termiantes the system

    @staticmethod
    def acquire(name: string) -> EspShader:
        # returns shader reference and loads it if necessary
        # name of shader has to be the same as the relative path 
        # to its spirv file without any extenions
        # the system will look for files with compatibile extensions
        # eg if you pass 'shaderpath/shader' the system will look for:
        # 'shaderpath/shader.vert.spv'
        # 'shaderpath/shader.tesc.spv'
        # 'shaderpath/shader.teste.spv'
        # 'shaderpath/shader.frag.spv'
        # 'shaderpath/shader.geom.spv'
        # 'shaderpath/shader.comp.spv'

    @staticmethod
    def release(name: string) -> None:
        # destroys shader resource and removes it from reference map

    @staticmethod
    def get_default_shader() -> EspShader:
        # returns default shader
```

```
class EspShader:
    def __init__(name: str):
        # shader constructor

    def create(name: str, spirv_resource: SpirvResource):
        # creates shader for active graphics pipeline

    def attach() -> None:
        # runs responsive method of EspWorker

    def create_uniform_manager(start_managed_ds: int = -1, end_managed_ds: int = -1) -> EspUniformManager:
        # runs responsive method of EspWorker

    def enable_depth_test(format: EspDepthBlockFormat, comapre_op: EspCompareOp) -> None:
        # runs responsive method of EspWorker

    def enable_multisampling(sample_count_flag: EspSampleCountFlag) -> None:
        # runs responsive method of EspWorker

    def set_attachment_formats(formats: list) -> None:
        # runs responsive method of EspWorker

    def set_vertex_layouts(vertex_layouts: list) -> None:
        # runs responsive method of EspWorker
        
    def set_worker_layout(uniforms_meta_data) -> None:
        # runs responsive method of EspWorker

    def build_worker() -> None:
        # runs responsive method of EspWorker
```

## Usage
```
def main() -> None:
    uniform_metadata = ...

    shader = ShaderSystem::acquire("Shaders/ObjExample/VikingRoomObjModelExample/shader");
    shader->enable_multisampling(EspSampleCountFlag::ESP_SAMPLE_COUNT_4_BIT);
    shader->enable_depth_test(EspDepthBlockFormat::ESP_FORMAT_D32_SFLOAT, EspCompareOp::ESP_COMPARE_OP_LESS);
    shader->set_vertex_layouts({ Mesh::Vertex::get_vertex_layout() });
    shader->set_worker_layout(std::move(uniform_meta_data));
    shader->build_worker();

    ...

    shader->attach()

    ...
```

# Material system

The amterial system is responsible for creating and handling materials. It conserves time and memory by caching materials and returning their references. Materials are cached by thier name and/or the vector of textures given material uses. Currently material system allows for up to 5 textures per material (albedo, normal, metallic, roughness and ao). Not every type of texture has to be passed to shader (configured via MaterialTexutreLayout). In case a type of texture isn not specified it's filled by references to default textures. A material holds reference to shader which is loaded with supplied textures. Material system class is a singleton and should be initialized at app start and terminated at app's exit.

```
class MaterialSystem:
    @staticmethod
    def create() -> MaterialSystem*:
        # initializes and returns material system instance

    def termiante() -> None:
        # termiantes the system

    @staticmethod
    def acquire(name: string) -> Material:
        # returns material reference with the same name
        # returns nullptr if no material with supplied name is found

    @staticmethod
    def acquire(textures: list, shader: EspShader, layouts: list) -> Material:
        # returns material reference with the same texutre list
        # if no material with same texture list is found a new material is created
        # this material will be searchable by texture list only
        # uses list of MetarialTextureLayout to determine which texutres to use and which 
        # descriptor sets and bindings will they occupy

    @staticmethod
    def acquire(textures: list, shader: EspShader, layouts: list) -> Material:
        # returns material reference with the same name
        # if there is no texture with supplied name returns material reference with the same texutre list
        # if no material with same texture list is found a new material is created
        # this material will be searchable by name and texutre list
        # uses list of MetarialTextureLayout to determine which texutres to use and which 
        # descriptor sets and bindings will they occupy

    @staticmethod
    def release(name: string) -> None:
        # destroys material and removes it from reference map (by name)

    @staticmethod
    def release(textures: list) -> None:
        # destroys material and removes it from reference map (by list of texutres)

    @staticmethod
    def release(name: string, textures: list) -> None:
        # destroys texture resource and removes it from reference map (by name and list of textures)
```

```
class MaterialTextureLayout
    def __init__(set: int, binding: int, type: EspTextureType):
        self.set = set
        self.binding = binding
        self.type = type
```

```
class Material:
    def __init__(texutres: list, shader: EspShader, layouts: list):
        # material constructor
        # creates uniform manager based on layouts

    def __init__(name: str, texutres: list, shader: EspShader, layouts: list):
        # material constructor
        # creates uniform manager based on layouts
        # sets material name

    def attach() -> None:
        # attaches material's buffer uniform
```

## Usage
```
def main() -> None:
    mesh = ...
    uniform_metadata = ...

    shader = ShaderSystem::acquire("Shaders/ObjExample/VikingRoomObjModelExample/shader");
    shader->enable_multisampling(EspSampleCountFlag::ESP_SAMPLE_COUNT_4_BIT);
    shader->enable_depth_test(EspDepthBlockFormat::ESP_FORMAT_D32_SFLOAT, EspCompareOp::ESP_COMPARE_OP_LESS);
    shader->set_vertex_layouts({ Mesh::Vertex::get_vertex_layout() });
    shader->set_worker_layout(std::move(uniform_meta_data));
    shader->build_worker();

    material = MaterialSystem::acquire({ TextureSystem::acquire("Models/viking_room/albedo.png") }, shader);
    mesh->set_material(material);

    ...

    shader->attach()
    mesh->draw() # calls material->attach()

    ...



```