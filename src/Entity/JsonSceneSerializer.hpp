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
            CloseMap(os);
        });

        CloseMap(os);

        os << closeBracket << newLine;
    }
private:
    static constexpr char comma        =  ',';
    static constexpr char quote        =  '\"';
    static constexpr char newLine      =  '\n';
    static constexpr char tabulation[] =  "  ";
    static constexpr char openBracket  =  '{';
    static constexpr char closeBracket =  '}';

    static unsigned int count;
    static unsigned int currentLayer;
    static std::unordered_map<unsigned int, unsigned int> varOnLayer;

    template<typename T>
    static void InsertVariable(std::ofstream& out, const std::string& variableName, const T& value)
    {
        if (varOnLayer[currentLayer] > 0)
        {
            out << comma;
        }
        out << newLine;
        varOnLayer[currentLayer] ++;

        for (int i = 0; i < count; ++i)
        {
            out << tabulation;
        }
        out << quote << variableName << quote << " : " << value;
    }

    static void InsertVariable(std::ofstream& out, const std::string& variableName, const glm::vec3& value)
    {
        if (varOnLayer[currentLayer] > 0)
        {
            out << comma;
        }
        out << newLine;
        varOnLayer[currentLayer] ++;

        for (int i = 0; i < count; ++i)
        {
            out << tabulation;
        }
        out << quote << variableName << quote << " : " << "[" << value.x << comma << " " << value.y << comma << " " << value.z << "]";
    }

    static void InsertVariable(std::ofstream& out, const std::string& variableName, const std::string& value)
    {
        if (varOnLayer[currentLayer] > 0)
        {
            out << comma;
        }
        out << newLine;
        varOnLayer[currentLayer] ++;

        for (int i = 0; i < count; ++i)
        {
            out << tabulation;
        }
        out << quote << variableName << quote << " : " << quote << value << quote;
    }

    static void StartMap(std::ofstream& out, const std::string& mapName)
    {
        if (varOnLayer[currentLayer] > 0)
        {
            out << comma;
        }
        out << newLine;
        varOnLayer[currentLayer] ++;
        currentLayer++;

        for (int i = 0; i < count; ++i)
        {
            out << tabulation;
        }
        out << quote << mapName << quote << " : " << newLine;

        for (int i = 0; i < count; ++i)
        {
            out << tabulation;
        }

        out << openBracket;
        count++;
    }

    static void CloseMap(std::ofstream& out)
    {
        varOnLayer[currentLayer] = 0;
        currentLayer--;
        
        out << newLine;
        for (int i = 0; i < count - 1; ++i)
        {
            out << tabulation;
        }
        out << closeBracket;
        count--;
    }

    static void SerializeEntity(std::ofstream& out, const Entity& entity)
    {
        if (entity.HasComponent<NameComponent>())
        {
            StartMap(out, "Name");
            const auto& component = entity.GetComponent<NameComponent>();
            InsertVariable(out, "Name", component.name);
            CloseMap(out);
        }

        if (entity.HasComponent<TransformComponent>())
        {
            StartMap(out, "Transform");
            const auto& component = entity.GetComponent<TransformComponent>();
            InsertVariable(out, "Position", component.translation);
            InsertVariable(out, "Rotation", component.rotation);
            InsertVariable(out, "Scale", component.scale);
            CloseMap(out);
        }

        if (entity.HasComponent<CameraComponent>())
        {
            StartMap(out, "Camera");
            const auto& component = entity.GetComponent<CameraComponent>();
            InsertVariable(out, "FOV", component.camera.GetFieldOfView());
            InsertVariable(out, "isPrimary", component.isPrimary);
            CloseMap(out);
        }

        if (entity.HasComponent<Model3DComponent>())
        {
            StartMap(out, "Model3D");
            const auto& component = entity.GetComponent<Model3DComponent>();
            InsertVariable(out, "Path", component.model.GetPath());
            CloseMap(out);
        }

        if (entity.HasComponent<DirectionalLightComponent>())
        {
            StartMap(out, "Directional light");
            const auto& component = entity.GetComponent<DirectionalLightComponent>();
            InsertVariable(out, "Ambient", component.directionalLight.GetAmbient());
            InsertVariable(out, "Diffuse", component.directionalLight.GetDiffuse());
            InsertVariable(out, "Specular", component.directionalLight.GetSpecular());
            InsertVariable(out, "Direction", component.directionalLight.GetDirection());
            CloseMap(out);
        }


        if (entity.HasComponent<PointLightComponent>())
        {
            StartMap(out, "Point light");
            const auto& component = entity.GetComponent<PointLightComponent>();
            InsertVariable(out, "Linear", component.pointLight.linear);
            InsertVariable(out, "Ambient", component.pointLight.ambient);
            InsertVariable(out, "Diffuse", component.pointLight.diffuse);
            InsertVariable(out, "Constant", component.pointLight.constant);
            InsertVariable(out, "Specular", component.pointLight.specular);
            InsertVariable(out, "Quadratic", component.pointLight.quadratic);
            CloseMap(out);
        }

    }
};

unsigned int JsonSceneSerializer::count = 1;
unsigned int JsonSceneSerializer::currentLayer = 0;
std::unordered_map<unsigned int, unsigned int> JsonSceneSerializer::varOnLayer;


#endif //GRAPHICS_JSONSCENESERIALIZER_HPP
