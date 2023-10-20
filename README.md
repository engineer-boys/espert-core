# Espert

## Build

In order to build espert several development tools are needed. Make sure they are installed and reachable from console.

Required packages:
- gcc and g++ or clang-17 and clang++-17
- make
- cmake
- git
- python3

All code dependecies are managed within project. To download all external modules run inside git repository:
```
git submodule update --init --recursive
```

To configure project run:
```
python3 scripts/espert-core.py configure
```

To build project run:
```
python3 scripts/espert-core.py build
```
Running configure before build is not necessary as the script will take care of it.

To run tests run:
```
python3 scripts/espert-core.py test
```
If no tests were run it means you have configured project without tests.
To reconfigure project with tests and run them use:
```
python3 scripts/espert-core.py -c test
```

## Formatting

To format code use:
```
python3 scripts/code-format.py
```
By default it runs in src directory. You can change the directory by adding -p flag.\
To fix coding style errors inplace add -i flag.