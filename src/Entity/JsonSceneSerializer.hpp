//
// Created by Anton on 16.09.2022.
//

#ifndef GRAPHICS_JSONSCENESERIALIZER_HPP
#define GRAPHICS_JSONSCENESERIALIZER_HPP

#include "Scene.h"
#include "Entity.h"

class JsonSceneSerializer
{
public:
    /**
     * Saves scene
     * @param savePath path to the save
     * @param scene scene to save
     */
    static void SaveScene(const std::string& savePath, Scene * scene)
    {
        std::ofstream os(savePath);

        GAME_ASSERT(os.is_open(), "Failed to open file " + savePath + " to save the scene");

        os << std::boolalpha;
        os << openBracket;

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

        os << newLine << closeBracket << newLine;

        // resetting variables
        count = 1;
        currentLayer = 0;
        varOnLayer.clear();
        os.close();
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

    /**
     * Inserts variable into the current map
     * @tparam T variable type
     * @param out stream to write into
     * @param variableName variable name
     * @param value variable value
     */
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

    /**
     * Inserts glm::vec3 variable into the current map
     * @param out stream to write into
     * @param variableName variable name
     * @param value variable value
     */
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

    /**
     * Inserts variable into the current map
     * @param out stream to write into
     * @param variableName variable name
     * @param value variable value
     */
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

    /**
     * Starts new map
     * @param out out stream to write imto
     * @param mapName new map name
     */
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

    /**
     * Closes current map
     * @param out out stream to write into
     */
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

    /**
     * Writes entity components to the stream
     * @param out out stream to write into
     * @param entity entity to serialize
     */
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
            InsertVariable(out, "Rotation", glm::degrees(component.rotation));
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
            InsertVariable(out, "Path", component.model.path);
            InsertVariable(out, "castsShadow", component.castsShadow);
            InsertVariable(out, "shouldBeLit", component.shouldBeLit);
            InsertVariable(out, "tilingFactor", component.tilingFactor);
            CloseMap(out);
        }

        if (entity.HasComponent<DirectionalLightComponent>())
        {
            StartMap(out, "Directional light");
            const auto& component = entity.GetComponent<DirectionalLightComponent>();
            InsertVariable(out, "Ambient", component.directionalLight.ambient);
            InsertVariable(out, "Diffuse", component.directionalLight.diffuse);
            InsertVariable(out, "Specular", component.directionalLight.specular);
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

        if (entity.HasComponent<EngineDefaultComponent>())
        {
            StartMap(out, "Engine default");
            const auto& component = entity.GetComponent<EngineDefaultComponent>();
            InsertVariable(out, "Type", component.type);
            CloseMap(out);
        }
    }
};

/**
 * Just to count number of tabulations on every new row
 */
inline unsigned int JsonSceneSerializer::count = 1;

/**
 * Depth index
 */
inline unsigned int JsonSceneSerializer::currentLayer = 0;

/**
 * Counts number of variables on the current depth
 */
inline std::unordered_map<unsigned int, unsigned int> JsonSceneSerializer::varOnLayer;


#endif //GRAPHICS_JSONSCENESERIALIZER_HPP
