# Resource system

The resource system is responsible for handling various engine resources. It's primary purpose is to deliver a simple API to load and unload certain resources. Resource system class is a singleton and should be initialized at app start and terminated at app's exit.
```
class ResourceSystem:
    @staticmethod
    def create(asset_base_path: Path) -> ResourceSystem*:
        # initialize resource system and return it's instance

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

    def get_data() -> void*:
        # return resource data

    def get_size() -> int:
        # return size of reosurce's data

    def release() -> void*:
        # release resource data 
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

## Usage - loading userdefined resource
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

    @staticmethod
    def acquire(name: string) -> Texture:
        # returns texture reference and loads it if necessary
        # name of texture has to be the same as the relative path 
        # to its asset file

    @staticmethod
    def release(name: string) -> None:
        # destroys texture resource and removes it from reference map

    @staticmethod
    def get_default_<type>_texture() -> Texture:
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