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