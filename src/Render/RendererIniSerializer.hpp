//
// Created by Anton on 14.10.2022.
//

#ifndef GRAPHICS_RENDERERINISERIALIZER_HPP
#define GRAPHICS_RENDERERINISERIALIZER_HPP

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <inipp.h>
#include "Renderer.h"
#include <glm/glm.hpp>

class RendererIniSerializer
{
public:
    static void SerializeRendererSettings()
    {
        if(!CheckIfFileExists())
        {
            CreateRendererIniFile();
        }

        std::ofstream fos(fileName);

        GAME_ASSERT(fos.is_open(), "Failed to open file " + fileName + " to save the scene");

        fos << std::boolalpha;

        StartSection(fos, "DEFAULT");

        AddVariable(fos, "fboWidth", Renderer::fboWidth);
        AddVariable(fos, "fboHeight", Renderer::fboHeight);
        AddVariable(fos, "isPostProcessingActivated", Renderer::isPostProcessingActivated);
        AddVariable(fos, "clearColor", Renderer::clearColor);
        AddVariable(fos, "cascadeLevels", Renderer::cascadeLevels);

        LOG(INFO) << "Renderer info serialized";
    }

    static void LoadRendererSettings()
    {
        std::ifstream is(fileName);

        // if file does not exist - create file and leave settings by default
        if(!is.is_open())
        {
            is.close();
            CreateRendererIniFile();
            return;
        }

        glm::vec3 clearColor;

        inipp::Ini<char> ini;

        ini.parse(is);
        ini.default_section(ini.sections["DEFAULT"]);
        ini.interpolate();

        auto& section = ini.sections["DEFAULT"];

        LoadVariable(section, "fboWidth", Renderer::fboWidth);
        LoadVariable(section, "fboHeight", Renderer::fboHeight);
        LoadVariable(section, "isPostProcessingActivated", Renderer::isPostProcessingActivated);
        LoadVariable(section, "clearColor", Renderer::clearColor);
        LoadVariable(section, "cascadeLevels", Renderer::cascadeLevels);

        is.close();

        LOG(INFO) << "Renderer settings loaded";
    }

    template<typename T>
    static void LoadVariable(const std::map<std::string, std::string>& section, const std::string& varName, T& destination)
    {
        inipp::get_value(section, varName, destination);
    }

    static void LoadVariable(const std::map<std::string, std::string>& section, const std::string& varName, glm::vec3& destination)
    {
        LoadArrayMember(section, varName, 0, destination.r);
        LoadArrayMember(section, varName, 1, destination.g);
        LoadArrayMember(section, varName, 2, destination.b);
    }

    template<typename T>
    static void LoadVariable(const std::map<std::string, std::string>& section, const std::string& varName, std::vector<T>& destination)
    {
        for(size_t i = 0; i < destination.size(); i++)
        {
            LoadArrayMember(section, varName, i, destination[i]);
        }
    }

    template<typename T>
    static void LoadArrayMember(const std::map<std::string, std::string>& section, const std::string& arrName, size_t index, T& destination)
    {
        const char openBracket  = '[';
        const char closeBracket = ']';

        std::stringstream ss;

        ss << arrName << openBracket << index << closeBracket;
        LoadVariable(section, ss.str(), destination);
    }


    template<typename T>
    static void AddVariable(std::ofstream& out, const std::string& name, const T& data)
    {
        out << newLineOperator << name << space << assignOperator << space << data;
    }

    static void AddVariable(std::ofstream& out, const std::string& name, const glm::vec3& data)
    {
        AddVariable(out, name, std::vector({data.x, data.y, data.z}));
    }

    template<typename T>
    static void AddVariable(std::ofstream& out, const std::string& name, const std::vector<T>& data)
    {
        const char openBracket  = '[';
        const char closeBracket = ']';

        for(size_t i = 0; i < data.size(); i++)
        {
            out << newLineOperator << name << openBracket << i << closeBracket << space << assignOperator << space << data[i];
        }
    }

    static bool CheckIfFileExists()
    {
        std::ifstream fis(fileName);
        bool exists = fis.is_open();
        fis.close();
        return exists;
    }

    static void CreateRendererIniFile()
    {
        std::ofstream _(fileName);
        _.close();
    }

    static void StartSection(std::ofstream& out, const std::string& name)
    {
        out << newLineOperator << startSection << name << closeSection;
    }

private:
    static const std::string fileName;
    static constexpr char space           = ' ';
    static constexpr char startSection    = '[';
    static constexpr char closeSection    = ']';
    static constexpr char assignOperator  = '=';
    static constexpr char newLineOperator = '\n';
};

inline const std::string RendererIniSerializer::fileName = "renderer.ini";

#endif //GRAPHICS_RENDERERINISERIALIZER_HPP
