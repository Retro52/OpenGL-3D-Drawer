//
// Created by Anton on 27.06.2022.
//

#ifndef GRAPHICS_APPLICATION_H
#define GRAPHICS_APPLICATION_H

#include <chrono>
#include <iostream>

class Application
{
public:
    /* Restriction to create an instance of this class */
    Application() = delete;
    Application(Application&&) = delete;
    Application(const Application&) = delete;

    /**
     * Creates application
     */
    static void Create();

    /**
     * Runs created application
     */
    static void Run();

    /**
     * Destroys application
     */
    static void Destroy();
};


#endif //GRAPHICS_APPLICATION_H
