//
// Created by Anton on 20.12.2021.
//

#include <iostream>
#include "ResourcesManager.h"

std::unique_ptr<Scene> ResourcesManager::pScene;
std::map<std::string, std::unique_ptr<Shader>> ResourcesManager::shaders;
std::mutex ResourcesManager::m;

void ResourcesManager::RegisterShader(const std::string &name, const std::string &vFile, const std::string &fFile, const std::string &gFile)
{
    const std::lock_guard<std::mutex> lock(m);
    try
    {
        if (!gFile.empty())
        {
            shaders[name] = std::make_unique<Shader>(vFile.c_str(), fFile.c_str(), gFile.c_str());
        }
        else
        {
            shaders[name] = std::make_unique<Shader>(vFile.c_str(), fFile.c_str());
        }
        LOG(INFO) << "Shader " + name + " successfully registered";
    }
    catch(InGameException& e)
    {
        LOG(WARNING) << "Failed to register shader" << name << ". Reason: " << e.what();
    }
}

Shader * ResourcesManager::GetShader(const std::string& name)
{
    const std::lock_guard<std::mutex> lock(m);
#if __cplusplus >= 202002L
    if(!shaders.contains(name))
    {
        LOG(ERROR) << "Shader " + name + " is not registered at ResourcesManager";
        throw InGameException("Trying to receive unregistered shader");
    }
#else
    /* Count returns number of elements with specified key, so we can use it like bool (non 0 - present, 0 - absent) */
    if(!shaders.count(name))
    {
        LOG(ERROR) << "Shader " + name + " is not registered at ResourcesManager";
        throw InGameException("Trying to receive unregistered shader");
    }
#endif
    return shaders[name].get();
}

void ResourcesManager::RegisterPlayerScene(const std::string &path)
{
    const std::lock_guard<std::mutex> lock(m);
    try
    {
        pScene = std::make_unique<Scene>(path);
    }
    catch(std::exception& e)
    {
        std::cerr << e.what();
    }
}

std::unique_ptr<Scene>& ResourcesManager::GetPlayerScene()
{
    return pScene;
}