//
// Created by Anton on 24.10.2022.
//

#ifndef GRAPHICS_UTILS_HPP
#define GRAPHICS_UTILS_HPP

#ifdef _WIN32
#include "windows.h"
constexpr const char * SCENE_FILE_FILTER = "Editor scene (*.json)\0*.json\0";
#else
constexpr const char* SCENE_FILE_FILTER = "";
#endif

#include <string>
#include <iostream>

namespace Utils
{
    /* Windows specific */
#ifdef _WIN32
    std::string OpenFile(const char * filter = "All Files (*.*)\0*.*\0", HWND owner = nullptr)
    {
        OPENFILENAME ofn;
        char fileName[MAX_PATH] = "";
        ZeroMemory(&ofn, sizeof(ofn));

        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = owner;
        ofn.lpstrFilter = filter;
        ofn.lpstrFile = fileName;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
        ofn.lpstrDefExt = "";

        std::string fileNameStr;

        if ( GetOpenFileName(&ofn) )
            fileNameStr = fileName;

        return fileNameStr;
    }

    std::string SaveFile(const char * filter = "All Files (*.*)\0*.*\0", HWND owner = nullptr)
    {
        OPENFILENAME ofn;
        char fileName[MAX_PATH] = "";
        ZeroMemory(&ofn, sizeof(ofn));

        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = owner;
        ofn.lpstrFilter = filter;
        ofn.lpstrFile = fileName;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
        ofn.lpstrDefExt = "";

        std::string fileNameStr;

        if ( GetSaveFileNameA(&ofn) )
            fileNameStr = fileName;

        return fileNameStr;
    }
#else
    std::string OpenFile(const char * filter = "All Files (*.*)\0*.*\0", HWND owner = nullptr)
    {
        std::string filename;
        std::cout << "Enter file name to open: \n";
        std::cin >> filename;
        return filename;
    }

    std::string SaveFile(const char * filter = "All Files (*.*)\0*.*\0", HWND owner = nullptr)
    {
        std::string filename;
        std::cout << "Enter file save path: \n";
        std::cin >> filename;
        return filename;
    }
#endif
}

#endif //GRAPHICS_UTILS_HPP
