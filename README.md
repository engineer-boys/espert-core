# Espert

## Build

In order to build espert several development tools are needed. Make sure they are installed and reachable from console.

Required packages:
- cmake
- git
- python3
Required packages for windows:
- MSVC 2022
Required packages for linux/MacOs:
- gcc and g++ or clang-17 and clang++-17
- make

All code dependecies are managed within project. To download all external modules run inside git repository:
```
git submodule update --init --recursive
```

To configure project run:
```
python3 scripts/espert-core.py configure
```
The default compilers are gcc/g++ for linux/MacOs and MSVC for windows.

To build project run:
```
python3 scripts/espert-core.py build
```
Running configure before build is not necessary as the script will take care of it. \
By default the project doesn't build Vulkan validation layers. If you want to run espert in debug mode and haven't installed Vulkan SDK add --vvl flag to build validation layers. Keep in mind that this process does take a WHILE. Please wait patiently and do not abort the build. Once you have build vvl one you won't have to do it again.

To run tests run:
```
python3 scripts/espert-core.py test
```
If no tests were run it means you have configured project without tests.
To reconfigure project with tests and run them use:
```
python3 scripts/espert-core.py -c test
```
To learn more about building options run
```
python3 scripts/espert-core.py --help
```

## Formatting

Required dependencies for code formatting are:
- clang-format-17
- clang-tidy-17

To format code use:
```
python3 scripts/code-format.py
```
By default it will run both clang-format and clang-tidy. Keep in mind that clang-tidy works only if you use clang and clang++ as project compilers. To rebuild project with clang run
```
python3 scripts/espert-core.py -c --clang build
```
By default it runs in src/ directory. You can change the directory by adding -p flag.\
To fix coding style errors inplace add -i flag. To fix suggestions add -x and to fix all errors add -e.\
To learn more about code formatting options run
```
python3 scripts/code-format.py --help
```

## Documentation

Documentation is generated with Doxygen.
In order to generate documentation add -x or --build-docs flag.
After build is completed open `build/docs/html/index.html` file to access documentation.