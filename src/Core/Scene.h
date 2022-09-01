//
// Created by Anton on 31.08.2022.
//

#ifndef GRAPHICS_SCENE_H
#define GRAPHICS_SCENE_H

#include <memory>
#include <mutex>
#include <queue>
#include <map>
#include "../Render/Model.h"
#include "../Lighting/PointLight.h"
#include "../Lighting/DirectionalLight.h"
#include "PerspectiveCamera.h"
#include "../Logging/easylogging++.h"
#include "InGameException.h"
//#include "ResourcesManager.h"

class Scene
{
public:
    explicit Scene(const std::string& path);
    ~Scene() = default;

    void Draw(int drawMode) const;

    void SwapCamera() {};
    void RemoveActor() {};
    void SwapDirLight() {};
    void RegisterActor() {};

    std::vector<std::unique_ptr<Actor>>& GetActors() { return actors; }
    std::unique_ptr<PerspectiveCamera>& GetPlayerCamera() { return pCamera; }

private:
    // maybe not really needed mutex
    std::mutex m;

    // scene content: lighting, actors and camera
    std::unique_ptr<DirectionalLight> dLight;
    std::vector<std::unique_ptr<Actor>> actors;
    std::unique_ptr<PerspectiveCamera> pCamera;
    std::vector<std::unique_ptr<PointLight>> pLights;
};


#endif //GRAPHICS_SCENE_H
