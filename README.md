# OpenGL-3D-Drawer

This is a simple 3D renderer, which can load and display almost any 3D model using OpenGL graphics API.

## Download

Renderer depends on GLEW, GLFW3, OpenGL32, FreeType, and ASSIMP libraries, and all of them, except ASSIMP and FreeType, are present in the include/ directory of the project. However, you not gonna need any of them as long as you will be using pre-built binaries.

## Build

To build the project you will have to install Assimp and FreeType libraries (note that you have to put FreeType library header in your default include folder, due to how the library is build) and run CMakeList.txt

## Running

Note that the renderer needs 2 config files (config.ini and config.json by default). You can specify the .json config file path in the .ini file, but not vice versa. You also might want to make sure that all of the relative paths in .json and .ini file are correct before running.

## Features
- The renderer supports loading models in a large number of formats (Wavefront .obj, .dae, .fbx, etc)
- Textures of almost any format are supported as well
- Lighting supports normal and specular maps
- Lighting supports multiple light-sources lighting
- Simple logging system is present and set-up
- And more
## License

MIT
**Long things short, that is a free-to-use code**