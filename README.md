# OpenFMC X3P Support Library

The core of this library is C++ code to read/write X3P files. We have also provided a MATLAB wrapper.

The source code is available at https://github.com/OpenFMC/x3p This repository is a repackaged and updated version of the code provided by the openGPS project http://open-gps.sourceforge.net The changes made are aimed at updating the CMake build chain to work without Subversion and more modern C++ compilers, and adding support for the vcpkg package manager. The library should build and run on Windows and Linux. macOS has not yet been tested. Patches, pull requests, etc. to improve the code or the build process are welcome.

## Build Environment

Use a current version of CMake. On Linux, use either a current GCC or Clang version. Also install pkg-config. On Windows, use Visual Studio 2022 in conjunction with vcpkg.

### CodeSynthesis XSD 4.2

Obtain the source or binary packages for xsd and libxsd from https://codesynthesis.com/products/xsd/download.xhtml Build and install the source packages or install the binary packages. To install on Windows, download the binary packages and unpack the contents into the same directory. Then set the environment variable `XSD_ROOT` so that it points to this directory.

### Xerces C++ 3.2

If available, install this library with your package manager, e.g. `libxerces-c-dev` under Ubuntu or `xerces-c` with vcpkg. Alternatively, you can also obtain the source code from https://xerces.apache.org

### minizip and zlib 

If the `BUILD_SHARED_LIBS` option is set to on, these dependencies are automatically downloaded from https://github.com/madler/zlib and linked as a static library. Otherwise, install these libraries with your package manager, e.g. `libminizip-dev` and `zlib1g-dev` under Ubuntu or `minizip` with vcpkg.

## Usage in CMake Projects

You can use the following instructions to integrate this library into your own CMake projects. For this to work, you must either have it installed on your system or set the `CMAKE_PREFIX_PATH` environment variable or the CMake variable `iso5436_2_xml_DIR` to point to the specific package location. In addition, Xerces C++ must be resolvable via `find_package`. For the C++ interface, the CodeSynthesis XSD headers must also be added to the target include directories.

```
find_package(iso5436_2_xml REQUIRED)
target_link_libraries([target] PRIVATE OPENGPS::iso5436_2_xml)
```
## MATLAB X3PToolbox

During the CMake configuration, set the `BUILD_MATLAB_TOOLBOX` option to on to build the MATLAB wrapper. An additional package will be created which contains the toolbox in the directory `X3PToolbox` together with some documentation and test scripts. On Linux it might be required to set the `Matlab_ROOT` variable so that CMake is able to detect your MATLAB installation e.g. `Matlab_ROOT=/usr/local/MATLAB/R2024a`.