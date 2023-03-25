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
#include "Camera.h"
#include "../Entity/Scene.h"
#include "Layer.hpp"


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
    static std::unique_ptr<Scene>& GetPlayerScene() { return pScene; };

    /**
     * Get registered shader by name
     * @param name shader name
     * @return pointer to the shader
     */
    static std::shared_ptr<Shader>& GetShader(const std::string &name);

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

    /**
     * Creates new application layer
     * @param layer Any class derived from Layer
     */
    static void RegisterLayer(const std::shared_ptr<Layer>& layer)
    {
        layer->OnCreate();
        layers.push_back(layer);
    }

    /**
     * Get one of the registered layers
     * @param index layer index
     * @return layer at index
     */
    static std::shared_ptr<Layer>& GetLayer(int index)
    {
        return layers.at(index);
    }

    static unsigned int GetNumOfLayers() { return layers.size(); }

    static std::vector<std::shared_ptr<Layer>>& GetLayers()
    {
        return layers;
    }

    static std::map<std::string, std::shared_ptr<Shader>>& GetShaders()
    {
        return shaders;
    }

    static void ShutDown()
    {
        // destroying the opened scene
        pScene = nullptr;

        // clearing layers
        for (const auto& layer : layers)
        {
            layer->OnDestroy();
        }
        layers.clear();

        // clearing shaders
        shaders.clear();
    }

private:
    static std::unique_ptr<Scene> pScene;
    static std::vector<std::shared_ptr<Layer>> layers;
    static std::map<std::string, std::shared_ptr<Shader>> shaders;

    static std::mutex m;
};

#endif //GRAPHICS_RESOURCESMANAGER_H
