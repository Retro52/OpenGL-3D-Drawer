# OpenGL-3D-Drawer

This simple 3D renderer can load and display almost any 3D model using OpenGL graphics API.

## Build
### Dependencies
The project depends on FreeType, Assimp, GLFW, GLEW, and OpenGL32 static libraries. All of them are present in the vendors folder.

To build the project you will have to install Assimp and FreeType libraries (note that you have to put the FreeType library header in your default include folder, due to how the library is built) and run CMakeList.txt

## Run

Note that the application needs 2 configuration files (config.ini and config.json by default). You can specify the .json config file path in the .ini file without recompiling, but not vice versa. You also might want to ensure that all of the relative paths in the .json and .ini files are correct before running.

## Features
- The renderer supports loading models in a large number of formats (Wavefront .obj, .dae, .fbx, etc)
- Textures of almost any format are supported as well
- Lighting supports normal and specular maps
- Lighting supports multiple light-sources lighting
- Directional lights soft shadows
- Entity system implemented using entt library
- Simple logging system is present and set-up
- And more
## License
MIT License
