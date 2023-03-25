cmake_minimum_required(VERSION 3.17)
project(Graphics)

set(CMAKE_CXX_STANDARD 17)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_SOURCE_DIR}/bin>)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Disabling min and max functions defined into the Windows.h header if compiling with MSVC (causes problems with std::numeric limits)
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_definitions(NOMINMAX)
    set(ASSIMP_LINK_LIBRARY "assimp-vc142-mtd")
else()
    set(ASSIMP_LINK_LIBRARY "assimp")
endif()


include_directories(vendors/include/GLFW)
link_directories(vendors/lib/GLFW)

include_directories(vendors/include/GLEW)
link_directories(vendors/lib/GLEW)

include_directories(vendors/include/imgui)

include_directories(vendors/include)

link_directories(vendors/lib/assimp)

set(ImGuizmoSource vendors/include/ImGuizmo/ImGuizmo.h vendors/include/ImGuizmo/ImGuizmo.cpp)
set(EditorSource src/Editor/EditorLayer.cpp src/Editor/EditorLayer.h)
set(InputSource src/Input/EventsHandler.cpp src/Input/EventsHandler.h)
set(LoggingSource src/Logging/easylogging++.cc src/Logging/easylogging++.h)
set(ImGuiSource vendors/include/imgui/imconfig.h vendors/include/imgui/imgui.h vendors/include/imgui/imgui.cpp vendors/include/imgui/imgui_demo.cpp vendors/include/imgui/imgui_draw.cpp vendors/include/imgui/imgui_internal.h vendors/include/imgui/imgui_tables.cpp vendors/include/imgui/imgui_widgets.cpp vendors/include/imgui/imstb_rectpack.h vendors/include/imgui/imstb_textedit.h vendors/include/imgui/imstb_truetype.h vendors/include/imgui/backends/imgui_impl_glfw.h vendors/include/imgui/backends/imgui_impl_glfw.cpp vendors/include/imgui/backends/imgui_impl_opengl3.h vendors/include/imgui/backends/imgui_impl_opengl3.cpp vendors/include/imgui/misc/cpp/imgui_stdlib.h vendors/include/imgui/misc/cpp/imgui_stdlib.cpp)
set(VendorsSource vendors/include/stb_image.h vendors/include/inipp.h vendors/include/json.hpp)
set(LightingSource src/Lighting/DirectionalLight.cpp src/Lighting/DirectionalLight.h src/Lighting/PointLight.cpp src/Lighting/PointLight.h )
set(RenderSource src/Render/Shader.h src/Render/Mesh.h src/Render/Model.h src/Render/Model.cpp src/Render/Mesh.cpp src/Render/Shader.cpp src/Render/Renderer.cpp src/Render/Renderer.h src/Render/UBO.hpp src/Render/FBO.hpp src/Render/SSBO.hpp src/Render/Material.cpp src/Render/Material.h src/Render/RendererIniSerializer.hpp)
set(CoreSource src/Core/Window.cpp src/Core/Window.h src/Core/Camera.cpp src/Core/Camera.h src/Core/ResourcesManager.cpp src/Core/ResourcesManager.h src/Core/Utils.hpp src/Core/MainLoop.cpp src/Core/MainLoop.h src/Core/Config.cpp src/Core/Config.h src/Core/EngineException.cpp src/Core/EngineException.h src/Core/Application.cpp src/Core/Application.h src/Core/UniqueID.hpp src/Core/UniqueIdGenerator.hpp src/Core/Layer.hpp src/Core/Event.hpp src/Core/Profiler.hpp)
set(EntitySource src/Entity/Entity.cpp src/Entity/Entity.h src/Entity/Scene.cpp src/Entity/Scene.h src/Entity/Components.h src/Entity/JsonSceneSerializer.hpp)
set(GameSource src/Game/GameLayer.cpp src/Game/GameLayer.h)

add_executable(Graphics
        ${CoreSource}
        ${GameSource}
        ${InputSource}
        ${ImGuiSource}
        ${EditorSource}
        ${EntitySource}
        ${RenderSource}
        ${VendorsSource}
        ${LoggingSource}
        ${ImGuizmoSource}
        ${LightingSource}
        src/main.cpp
        )

target_link_libraries(Graphics glew32s OpenGL32.lib glfw3 ${ASSIMP_LINK_LIBRARY})

add_custom_command(TARGET Graphics POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/vendors/bin"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")