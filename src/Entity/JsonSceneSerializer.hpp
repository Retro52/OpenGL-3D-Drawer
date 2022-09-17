//
// Created by Anton on 16.09.2022.
//

#ifndef GRAPHICS_JSONSCENESERIALIZER_HPP
#define GRAPHICS_JSONSCENESERIALIZER_HPP

#include "Scene.h"

class JsonSceneSerializer
{
public:
    static void SaveScene(const std::string& savePath, Scene * scene)
    {
        std::ofstream os(savePath);

        GAME_ASSERT(os.is_open(), "Failed to open file " + savePath + " to save the scene");

        os << std::boolalpha;
        os << openBracket << newLine;

        InsertVariable(os, "Name", (const std::string&) "SceneDefaultName");

        StartMap(os, "Entities");

        scene->registry.each([&](auto id)
        {
            auto u32id = (uint32_t) id;
            StartMap(os, "Entity" + std::to_string(u32id));
            // implicitly creating Entity using id gathered from registry, then serializing it to the json file
            SerializeEntity(os, { id, scene });
            CloseMap(os, u32id != 0);
        });

        CloseMap(os, false);

        os << closeBracket << newLine;
    }
private:
    static constexpr char comma        =  ',';
    static constexpr char quote        =  '\"';
    static constexpr char newLine      =  '\n';
    static constexpr char tabulation   =  '\t';
    static constexpr char openBracket  =  '{';
    static constexpr char closeBracket =  '}';
    static unsigned int count;

    template<typename T>
    static void InsertVariable(std::ofstream& out, const std::string& variableName, const T& value, bool shouldInsertComma = true)
    {
        for (int i = 0; i < count; ++i)
        {
            out << tabulation;
        }
        out << quote << variableName << quote << " : " << value;
        if (shouldInsertComma)
        {
            out << comma;
        }
        out << newLine;
    }

    static void InsertVariable(std::ofstream& out, const std::string& variableName, const glm::vec3& value, bool shouldInsertComma = true)
    {
        for (int i = 0; i < count; ++i)
        {
            out << tabulation;
        }
        out << quote << variableName << quote << " : " << "[" << value.x << comma << " " << value.y << comma << " " << value.z << "]";

        if (shouldInsertComma)
        {
            out << comma;
        }
        out << newLine;
    }

    static void InsertVariable(std::ofstream& out, const std::string& variableName, const std::string& value, bool shouldInsertComma = true)
    {
        for (int i = 0; i < count; ++i)
        {
            out << tabulation;
        }
        out << quote << variableName << quote << " : " << quote << value << quote;

        if (shouldInsertComma)
        {
            out << comma;
        }
        out << newLine;
    }

    static void StartMap(std::ofstream& out, const std::string& mapName)
    {
        for (int i = 0; i < count; ++i)
        {
            out << tabulation;
        }
        out << quote << mapName << quote << " : " << newLine;

        for (int i = 0; i < count; ++i)
        {
            out << tabulation;
        }

        out << openBracket << newLine;
        count++;
    }

    static void CloseMap(std::ofstream& out, bool shouldInsertComma = true)
    {
        for (int i = 0; i < count - 1; ++i)
        {
            out << tabulation;
        }
        out << closeBracket;

        if (shouldInsertComma)
        {
            out << comma;
        }
        out << newLine;
        count--;
    }

    static void SerializeEntity(std::ofstream& out, const Entity& entity)
    {
        unsigned int componentsCount = CalculateEntityComponentsCount(entity);
        if (entity.HasComponent<NameComponent>())
        {
            componentsCount--;
            StartMap(out, "Name");
            const auto& component = entity.GetComponent<NameComponent>();
            InsertVariable(out, "Name", component.name, false);
            CloseMap(out, componentsCount > 0);
        }

        if (entity.HasComponent<TransformComponent>())
        {
            componentsCount--;

            StartMap(out, "Transform");
            const auto& component = entity.GetComponent<TransformComponent>();
            InsertVariable(out, "Position", component.translation);
            InsertVariable(out, "Rotation", component.rotation);
            InsertVariable(out, "Scale", component.scale, false);
            CloseMap(out, componentsCount > 0);
        }

        if (entity.HasComponent<CameraComponent>())
        {
            componentsCount--;

            StartMap(out, "Camera");
            const auto& component = entity.GetComponent<CameraComponent>();
            InsertVariable(out, "FOV", component.camera.GetFieldOfView());
            InsertVariable(out, "isPrimary", component.isPrimary, false);
            CloseMap(out, componentsCount > 0);
        }

        if (entity.HasComponent<Model3DComponent>())
        {
            componentsCount--;

            StartMap(out, "Model3D");
            const auto& component = entity.GetComponent<Model3DComponent>();
            InsertVariable(out, "Path", component.model.GetPath(), false);
            CloseMap(out, componentsCount > 0);
        }

        if (entity.HasComponent<DirectionalLightComponent>())
        {
            componentsCount--;

            StartMap(out, "Directional light");
            const auto& component = entity.GetComponent<DirectionalLightComponent>();
            InsertVariable(out, "Ambient", component.directionalLight.GetAmbient());
            InsertVariable(out, "Diffuse", component.directionalLight.GetDiffuse());
            InsertVariable(out, "Specular", component.directionalLight.GetSpecular());
            InsertVariable(out, "Direction", component.directionalLight.GetDirection(), false);
            CloseMap(out, componentsCount > 0);
        }


        if (entity.HasComponent<PointLightComponent>())
        {
            componentsCount--;

            StartMap(out, "Point light");
            const auto& component = entity.GetComponent<PointLightComponent>();
            InsertVariable(out, "Linear", component.pointLight.linear);
            InsertVariable(out, "Ambient", component.pointLight.ambient);
            InsertVariable(out, "Diffuse", component.pointLight.diffuse);
            InsertVariable(out, "Constant", component.pointLight.constant);
            InsertVariable(out, "Specular", component.pointLight.specular);
            InsertVariable(out, "Quadratic", component.pointLight.quadratic, false);
            CloseMap(out, componentsCount > 0);
        }

    }

    static unsigned int CalculateEntityComponentsCount(const Entity& entity)
    {
        unsigned int compCount = 0;
        if (entity.HasComponent<NameComponent>())
        {
            compCount++;
        }
        if (entity.HasComponent<CameraComponent>())
        {
            compCount++;
        }
        if (entity.HasComponent<Model3DComponent>())
        {
            compCount++;
        }
        if (entity.HasComponent<TransformComponent>())
        {
            compCount++;
        }
        if (entity.HasComponent<PointLightComponent>())
        {
            compCount++;
        }
        if (entity.HasComponent<DirectionalLightComponent>())
        {
            compCount++;
        }
        return compCount;
    }
};

unsigned int JsonSceneSerializer::count = 1;

#endif //GRAPHICS_JSONSCENESERIALIZER_HPP
