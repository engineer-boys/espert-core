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