# The Chameleon

## Build using CMake
```
brew install pkg-config
brew install glfw3
brew install sdl2
brew install sdl2_mixer
brew install freetype
```
If CMake is not already installed, you can download and install from the CMake website: https://cmake.org/download/; or use the package manager of your choice.
Create an empty directory as the build directory, which we assume is named build. You can configure the project using CMake GUI or the command line.
For the GUI, enter the assignment template folder (which should contain a CMakeLists.txt file) as Source and the build folder as the Build. Then, press configure, and if the configuration if successful, press generate. For the command line, cd inside the build and run:
```
cmake [path_of_assignment_template] -DCMAKE_BUILD_TYPE=[Debug|Release]
```
Now you can build the generated project using make.