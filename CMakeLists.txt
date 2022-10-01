cmake_minimum_required(VERSION 3.17)
project(Graphics)

set(CMAKE_CXX_STANDARD 17)

include_directories(vendors/include/GLFW)
link_directories(vendors/lib/GLFW)

include_directories(vendors/include/GLEW)
link_directories(vendors/lib/GLEW)

include_directories(vendors/include/freetype2)
include_directories(vendors/include/imgui)

include_directories(vendors/include)


add_executable(Graphics src/main.cpp src/Core/Window.cpp src/Core/Window.h src/Input/EventsHandler.cpp src/Input/EventsHandler.h src/Core/PerspectiveCamera.cpp src/Core/PerspectiveCamera.h src/Render/Shader.h src/Render/Mesh.h src/Core/Structures.h src/Core/ResourcesManager.cpp src/Core/ResourcesManager.h src/Lighting/DirectionalLight.cpp src/Lighting/DirectionalLight.h src/Lighting/PointLight.cpp src/Lighting/PointLight.h src/Core/Global.cpp src/Core/Global.h src/Logging/easylogging++.cc src/Logging/easylogging++.h src/UI/UIHandler.cpp src/UI/UIHandler.h vendors/include/inipp.h src/Core/Config.cpp src/Core/Config.h vendors/include/json.hpp src/Render/Model.h vendors/include/stb_image.h src/Render/Model.cpp src/Render/Mesh.cpp src/Render/Shader.cpp src/Core/InGameException.cpp src/Core/InGameException.h src/Core/Application.cpp src/Core/Application.h src/Lighting/ShadowsHandler.cpp src/Lighting/ShadowsHandler.h src/Entity/Entity.cpp src/Entity/Entity.h src/Entity/Scene.cpp src/Entity/Scene.h src/Entity/Components.h src/Render/Renderer.cpp src/Render/Renderer.h src/Render/UBO.hpp src/Render/FBO.hpp src/Entity/JsonSceneSerializer.hpp src/Render/Material.cpp src/Render/Material.h src/Core/UniqueID.hpp src/Core/UniqueIdGenerator.hpp src/Core/Layer.hpp src/Editor/EditorLayer.cpp src/Editor/EditorLayer.h)

target_link_libraries(Graphics libglew32 OpenGL32.lib libglfw3.a freetype.lib assimp.lib)
