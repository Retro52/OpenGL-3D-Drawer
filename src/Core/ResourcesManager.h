//
// Created by Anton on 20.12.2021.
//

#ifndef GRAPHICS_RESOURCESMANAGER_H
#define GRAPHICS_RESOURCESMANAGER_H

#include <memory>
#include <mutex>
#include <queue>
#include <map>
#include "../Render/Model.h"
#include "../Lighting/PointLight.h"
#include "../Lighting/DirectionalLight.h"
#include "PerspectiveCamera.h"
#include "../Entity/Scene.h"


class ResourcesManager
{
public:
    /* Restriction to create an instance of this class */
    ResourcesManager() = delete;
    ResourcesManager(ResourcesManager&&) = delete;
    ResourcesManager(const ResourcesManager&) = delete;

    /**
     * Returns current scene
     * @return current scene
     */
    static std::unique_ptr<Scene>& GetPlayerScene();

    /**
     * Get registered shader by name
     * @param name shader name
     * @return pointer to the shader
     */
    static Shader * GetShader(const std::string &name);

    /**
     * Register new shader
     * @param name shader name
     * @param vFile path to the vertex shader file
     * @param fFile path to the fragment shader file
     * @param gFile path to the geometry shader file (optional)
     */
    static void RegisterShader(const std::string &name, const std::string &vFile, const std::string &fFile, const std::string &gFile = "");

    /**
     * Registers current scene
     * @param path path to the scene to load
     */
    static void RegisterPlayerScene(const std::string& path);

private:
    static std::unique_ptr<Scene> pScene;
    static std::map<std::string, std::unique_ptr<Shader>> shaders;

    static std::mutex m;
};

#endif //GRAPHICS_RESOURCESMANAGER_H
