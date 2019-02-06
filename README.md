# Qt-CreatorTMC
[![Build Status](https://travis-ci.org/TestMyQt/Qt-CreatorTMC.svg?branch=master)](https://travis-ci.org/TestMyQt/Qt-CreatorTMC)

**Note: Currently the most up-to-date branch is not master, checkout `tqc-tmc`**

## Building the plugin by yourself (Linux)

1. Install Qt Creator [download.qt.io](http://download.qt.io/official_releases/qtcreator/) if you haven't got it already.
2. Get the sources for the Qt Creator build from [code.qt.io](http://code.qt.io/cgit/qt-creator/qt-creator.git/)
    - `git clone git://code.qt.io/qt-creator/qt-creator.git`
3. Qt Creator is released from tags, like 'v4.8.0-beta2', so checkout the right Qt Creator version tag:
    - `git checkout v4.8.1`
4. Clone this repository recursively: `git clone --recursive https://github.com/testmyqt/qt-creatortmc`
    - `3rdparty` contains sources for dependencies that are statically-linked to the plugin shared object file: [`quazip`](https://github.com/TestMyQt/quazip/tree/eeb61805596a83fd8a97f542f48430485330217a) and [`zlib`](https://github.com/madler/zlib/tree/cacf7f1d4e3d44d871b605da3b647f07d718623f)
5. The project can be opened in Qt Creator now. To configure the build target set the following environment variables:
    - To set environment variables in Qt Creator, go to Projects Mode -> Build -> Build Environment
    - `QTC_BUILD` your Qt Creator build
    - `QTC_SOURCE` your Qt Creator sources
6. Build the project. The plugin will be located in `QTC_BUILD/lib/qtcreator/plugins/libTestMyCode.so`
7. Restart Qt Creator to load the plugin.


## Development setup

If you wish to develop the plugin, you might want a setup with a second instance of Creator. One way to accomplish this is to build Creator by yourself.

1. Get the sources for the Qt Creator build from [code.qt.io](http://code.qt.io/cgit/qt-creator/qt-creator.git/)
    - `git clone git://code.qt.io/qt-creator/qt-creator.git`
    - `cd qt-creator`
    - Qt Creator is released from tags, like `v4.8.0-beta2`, so checkout the right Qt Creator version tag:
    - `git checkout v4.8.0-beta2`
    - Note: **You will need a debug build if you want to debug your project**
2. To build Qt Creator in to a separate build directory:
    - `mkdir ../qt-creator-build && cd ../qt-creator-build`
    - `qmake ../qt-creator`
    - `make` or `make -j $(nproc)` to build on all cores
3. Clone this repository recursively: `git clone --recursive https://github.com/testmyqt/qt-creatortmc`
    -`3rdparty` contains sources for dependencies that are statically-linked to the plugin shared object file:
    - [`quazip`](https://github.com/TestMyQt/quazip/tree/eeb61805596a83fd8a97f542f48430485330217a)
    - [`zlib`](https://github.com/madler/zlib/tree/cacf7f1d4e3d44d871b605da3b647f07d718623f)
4. The project can be opened in Qt Creator now. To configure the build target set the following environment variables:
    - To set environment variables in Qt Creator, go to Projects Mode -> Build -> Build Environment
    - `QTC_BUILD` your Qt Creator build
    - `QTC_SOURCE` your Qt Creator sources
    - `BUILD_OUTPUT_PATH` where the shared library file will be located. If not set, will install to `QTC_BUILD` dir.

### Linux
- Debug build `qmake -spec linux-g++ CONFIG+=debug`
- Test build `qmake -spec linux-g++ CONFIG+=test`
- Run `make` to build
- Run `make install` to install the plugin `.so` file to the plugin directory

To run the plugin/IDE, you need to create a Run configuration to launch the `QTC_BUILD`, Projects Mode -> Run. It is recommended to add the additional command line argument `-settingspath path-to-temporary-settings` to not run in the same creator session as your development environment.

## Generating QDoc
The QDoc configuration file is `main.qdocconf` in the root directory of the project. The QDoc documentation is automatically generated into `doc/html` by running the command `qdoc main.qdocconf`. Note that each time the command is run, all the previous content of `doc/html` is deleted. So don't place any manually generated content into the directory!
