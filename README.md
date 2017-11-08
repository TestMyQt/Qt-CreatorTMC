# Qt-CreatorTMC
[![Build Status](https://travis-ci.org/TestMyQt/Qt-CreatorTMC.svg?branch=master)](https://travis-ci.org/TestMyQt/Qt-CreatorTMC)

[backlog](https://docs.google.com/spreadsheets/d/1SZ1n11YmaVU5PIb6m-NHczgdKhNN1TSwjyNvZwEiMIg/edit#gid=0)

## Development setup

Clone the `master` repository recursively: `git clone --recursive https://github.com/testmyqt/qt-creatortmc`

`3rdparty` contains sources for dependencies that are statically-linked to the plugin shared object file:
- [`quazip`](https://github.com/TestMyQt/quazip/tree/eeb61805596a83fd8a97f542f48430485330217a)
- [`zlib`](https://github.com/madler/zlib/tree/cacf7f1d4e3d44d871b605da3b647f07d718623f)

_Environment variables_:

- `QTC_BUILD` your Qt Creator build
- `QTC_SOURCE` your Qt Creator sources
- `BUILD_OUTPUT_PATH` where the shared library file will be located. If not set, will install to `QTC_BUILD` dir.

## Linux
- Debug build `qmake -spec linux-g++ CONFIG+=debug`
- Test build `qmake -spec linux-g++ CONFIG+=test`
- Run `make` to install the plugin `.so` file to the plugin directory
