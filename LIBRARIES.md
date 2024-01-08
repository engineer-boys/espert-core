# Libraries

The espert engine relies on multiple other projects. Their respective repositories have been added as git submodules in the `external` directory. Every library is build automatically when `scripts/espert-core.py` is run. After initial build the libraries are cached in `libs` directory. To rebuild all external libraries run the build script with `--hard-clean` flag as `--clean` causes only espert files to be rebuilt. In case of any problems you can manually remove library directory or install it with `scripts/install-library.py`.

## Used libraries

- [Catch2](https://github.com/catchorg/Catch2.git)
- [Doxygen](https://github.com/doxygen/doxygen.git)
- [entt](https://github.com/skypjack/entt)
- [glfw](https://github.com/glfw/glfw)
- [glm](https://github.com/g-truc/glm)
- [glslang](https://github.com/KhronosGroup/glslang/)
- [spdlog](gttps://github.com/gabime/spdlog)
- [stb](https://github.com/nothings/stb)
- [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader)
- [volk](https://github.com/zeux/volk)
- [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers)
- [Vulka-ValidationLayers](https://github.com/KhronosGroup/Vulkan-ValidationLayers)

## Adding libraries

- Add library repository as a submodule - `git submodule add <link> external/<name>`.
- Add library config entry in `ESPERT_LIBS_CONF` dictionary in `scripts/libconf.py`. The key should be same as the name of library repository and the value a list of `CmakeParameter` to use during library  build step (can be left empty).
- Build library by running `python3 scripts/install-library.py <name>`.
- Navigate the `libs` directory. There should be an entry for the new library. Enter it and then the `Debug` directory. Starting here look for some `*.cmake` files (eg Catch2Targets.cmake) and remember their location relative to `Debug` directory.
- Open the `<name>Targets.cmake` file and look for target declaration you're interested in (add_library(...)). Remember the target name.
- Go to `CmakeLists.txt` file and add the folowing entry:
```
get_library(NAME <repository name>
    CONF_PATH <relative path to cmake files>
    CONF_NAMES <alternative library names>
)
```
If the `name` in `<name>Targets.cmake` is the same as repository name you can delete the `CONF_NAMES` argument. Otherwise pass this `name` to `CONF_NAMES` argument.
- Add the target name you have remembered to `target_link_libraries` in `CMakeLists.txt`.