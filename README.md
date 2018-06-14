# Qt-CreatorTMC
[![Build Status](https://travis-ci.org/TestMyQt/Qt-CreatorTMC.svg?branch=master)](https://travis-ci.org/TestMyQt/Qt-CreatorTMC)

## Development setup

- Get a Qt Creator build
  - ~Official binary release from [download.qt.io](http://download.qt.io/official_releases/qtcreator/)~ This option is not yet available due to release builds not having the patch need for the plugin. Release version 4.7 should have it in the future.
  - Compile it yourself from source (takes a while, 30-45min)
  - Note: **You will need a debug build if you want to debug your project**
- Get the sources for the Qt Creator build from [code.qt.io](http://code.qt.io/cgit/qt-creator/qt-creator.git/)
  - `git clone --depth 1 --branch $QTC_BUILD_VERSION git://code.qt.io/qt-creator/qt-creator.git`

### Build Qt Creator

Due to the patch we need, you'll need to compile the Qt Creator 4.7 development branch.

To build 4.7 in to a separate build directory:

- `git clone --depth 1 --branch 4.7 git://code.qt.io/qt-creator/qt-creator.git qt-creator-4.7`
- `mkdir qt-creator-4.7-build && cd qt-creator-4.7-build`
- `qmake ../qt-creator-4.7`
- `make` or `make -j $(nproc)` to build on all cores

### Compile the plugin

Clone the `master` repository recursively: `git clone --recursive https://github.com/testmyqt/qt-creatortmc`

`3rdparty` contains sources for dependencies that are statically-linked to the plugin shared object file:
- [`quazip`](https://github.com/TestMyQt/quazip/tree/eeb61805596a83fd8a97f542f48430485330217a)
- [`zlib`](https://github.com/madler/zlib/tree/cacf7f1d4e3d44d871b605da3b647f07d718623f)

Project can be opened in Qt Creator. To configure the build target, set the following environment variables:

- `QTC_BUILD` your Qt Creator build
- `QTC_SOURCE` your Qt Creator sources
- `BUILD_OUTPUT_PATH` where the shared library file will be located. If not set, will install to `QTC_BUILD` dir.

To set environment variables in Qt Creator, go to Projects Mode -> Build -> Build Environment

## Linux
- Debug build `qmake -spec linux-g++ CONFIG+=debug`
- Test build `qmake -spec linux-g++ CONFIG+=test`
- Run `make` to build
- Run `make install` to install the plugin `.so` file to the plugin directory

To run the plugin/IDE, you need to create a Run configuration to launch the `QTC_BUILD`, Projects Mode -> Run. It is recommended to add the additional command line argument `-settingspath path-to-temporary-settings` to not run in the same creator session as your development environment.

## Generating QDoc
The QDoc configuration file is `main.qdocconf` in the root directory of the project. The QDoc documentation is automatically generated into `doc/html` by running the command `qdoc main.qdocconf`. Note that each time the command is run, all the previous content of `doc/html` is deleted. So don't place any manually generated content into the directory!
