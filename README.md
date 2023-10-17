# Espert

## Build

In order to build espert several development tools are needed. Make sure they are installed and reachable from console.

Required packages:
- gcc/g++
- make
- cmake
- git

All code dependecies are managed within project. To download all external modules run in git repository:
```
git submodule update --init --recursive
```

To setup project run configure script for your system (unix-configure.sh or windows-configure.bat).

To build project run make command inside build directory after configuring project. This will produce a static library inside build directory.

## Testing

To enable tests run cmake with ESP_BUILD_TESTS set to ON. \
Tests are automatically registered with cTest so you just need to run ctest command from build directory. \
All tests should be placed in tests directory.