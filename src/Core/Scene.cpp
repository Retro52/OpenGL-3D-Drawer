//
// Created by Anton on 31.08.2022.
//

#include "Scene.h"
#include "../Loaders/json.hpp"
#include "ResourcesManager.h"
#include "../Lighting/ShadowsHandler.h"

Scene::Scene(const std::string &path)
{
    LOG(INFO) << "Start loading scene " << path << " config file";
    std::ifstream is(path);
    if (!is.is_open())
    {
        LOG(ERROR) << "Failed to open config file '" << path << "'";
        throw InGameException("Failed to load " + path + " scene file");
    }

    json data = json::parse(is);

    LOG(INFO) << "Scene name: " << data["SceneName"];

    for (const auto& model : data["Models"].items())
    {
        try
        {
            actors.push_back(std::make_unique<Model>(model.value()["Path"]));
            actors.back()->ScaleTo(glm::vec3(model.value()["Scale"][0], model.value()["Scale"][1], model.value()["Scale"][2]));
            actors.back()->MoveTo(glm::vec3(model.value()["Position"][0], model.value()["Position"][1], model.value()["Position"][2]));
            actors.back()->RotateTo(glm::vec3(model.value()["Rotation"][0], model.value()["Rotation"][1], model.value()["Rotation"][2]));
            LOG(INFO) << "Model " << model.key() << " loaded";
        }
        catch (InGameException& e)
        {
            LOG(WARNING) << "Failed to load model " << model.key() << " . Reason: " << e.what();
        }
    }

    for (const auto& model : data["Point lights"].items())
    {
        try
        {
            glm::vec3 amb(model.value()["Ambient"][0], model.value()["Ambient"][1], model.value()["Ambient"][2]);
            glm::vec3 diff(model.value()["Diffuse"][0], model.value()["Diffuse"][1], model.value()["Diffuse"][2]);
            glm::vec3 pos(model.value()["Position"][0], model.value()["Position"][1], model.value()["Position"][2]);
            glm::vec3 spec(model.value()["Specular"][0], model.value()["Specular"][1], model.value()["Specular"][2]);
            float con = model.value()["Constant"];
            float lin = model.value()["Linear"];
            float quad = model.value()["Quadratic"];
            pLights.push_back(std::make_unique<PointLight>(pos, diff, amb, spec, con, lin, quad));
            LOG(INFO) << "Point light " << model.key() << " registered";
        }
        catch (InGameException& e)
        {
            LOG(WARNING) << "Failed to load point light " << model.key() << " . Reason: " << e.what();
        }
    }

    try
    {
        const auto & dLightData = data["Directional light"];
        glm::vec3 amb(dLightData["Ambient"][0], dLightData["Ambient"][1], dLightData["Ambient"][2]);
        glm::vec3 diff(dLightData["Diffuse"][0], dLightData["Diffuse"][1], dLightData["Diffuse"][2]);
        glm::vec3 dir(dLightData["Direction"][0], dLightData["Direction"][1], dLightData["Direction"][2]);
        glm::vec3 spec(dLightData["Specular"][0], dLightData["Specular"][1], dLightData["Specular"][2]);
        dLight = std::make_unique<DirectionalLight>(dir, diff, amb, spec);
        LOG(INFO) << "Directional light loaded";
    }
    catch(InGameException& e)
    {
        LOG(WARNING) << "Failed to load directional light. Reason: " << e.what();
    }

    try
    {
        const auto & pCameraData = data["Camera"];
        glm::vec3 pos(pCameraData["Position"][0], pCameraData["Position"][1], pCameraData["Position"][2]);
        pCamera = std::make_unique<PerspectiveCamera>(pos, glm::radians(static_cast<float>(pCameraData["FOV"])));
        LOG(INFO) << "Camera loaded";
    }
    catch(InGameException& e)
    {
        LOG(WARNING) << "Failed to load camera. Reason: " << e.what();
    }
}

void Scene::Draw(int drawMode) const
{
    Shader * mainShader   = ResourcesManager::GetShader("mainShader");
    Shader * shadowShader = ResourcesManager::GetShader("shadowShader");

    glm::mat4 lightOrtho = glm::ortho(-35.0f, 35.0f, 35.0f, -35.0f, 0.1f, 75.0f);
    glm::mat4 lightView = glm::lookAt(20.f * (- dLight->direction), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightProjection = lightOrtho * lightView;

    unsigned int shadowTexture;

    shadowShader->Use();
    shadowShader->setMat4("lightProjection", lightProjection);

    if (drawMode == 2 || (drawMode > 6 && drawMode < 9) )
    {
        shadowTexture = 0;
    }
    else
    {
        shadowTexture = ShadowsHandler::RenderShadowMap();
    }

    glm::mat4 projView = pCamera->GetProjection() * pCamera->GetView();

    glClearColor(0.203f, 0.76f, 0.938f,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (drawMode == 4)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    mainShader->Use();
    mainShader->setInt("drawMode", drawMode);
    mainShader->setMat4("view", pCamera->GetView());
    mainShader->setVec3("ProjPos", pCamera->GetPosition());
    mainShader->setMat4("projection", pCamera->GetProjection());
    mainShader->setMat4("lightProjection", lightProjection);
    mainShader->setPointLights(pLights);
    mainShader->setDirLight(dLight);

    for(auto &a : actors)
    {
        a->Tick();
        a->UpdateControls();
        a->Draw(* mainShader, shadowTexture);
    }
}
