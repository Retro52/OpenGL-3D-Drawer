//
// Created by Anton on 29.10.2022.
//

#ifndef GRAPHICS_PROFILER_HPP
#define GRAPHICS_PROFILER_HPP

#include <chrono>

// based on this: https://stackoverflow.com/a/21995693/14504988
template <class DT = std::chrono::microseconds, class ClockT = std::chrono::high_resolution_clock>
class Timer
{
    using timep_t = typename ClockT::time_point;
    timep_t _start = ClockT::now(), _end = {};

public:
    void tick()
    {
        _end = timep_t{};
        _start = ClockT::now();
    }

    void tock()
    {
        _end = ClockT::now();
    }

    template <class T = DT>
    [[nodiscard]] auto duration() const
    {
        ASSERT(_end != timep_t{}, "Timer: toc before reporting");
        return std::chrono::duration_cast<T>(_end - _start);
    }
};

class Profiler
{
public:
    static void StartCpu()
    {
        cpuTimer.tick();
    }

    static void EndCpu()
    {
        cpuTimer.tock();
    }

    static void StartGPass()
    {
        gTimer.tick();
    }

    static void EndGPass()
    {
        gTimer.tock();
    }

    static void StartSPass()
    {
        sTimer.tick();
    }

    static void EndSPass()
    {
        sTimer.tock();
    }

    static void StartLPass()
    {
        lTimer.tick();
    }

    static void EndLPass()
    {
        lTimer.tock();
    }

    static void StartDrawPrep()
    {
        prepTimer.tick();
    }

    static void EndDrawPrep()
    {
        prepTimer.tock();
    }

    static unsigned int totalMeshes, totalVertices;
    static Timer<std::chrono::milliseconds, std::chrono::steady_clock> cpuTimer, prepTimer, gTimer, sTimer, lTimer;
};

inline unsigned int Profiler::totalMeshes, Profiler::totalVertices;
inline Timer<std::chrono::milliseconds, std::chrono::steady_clock> Profiler::cpuTimer, Profiler::prepTimer, Profiler::gTimer, Profiler::sTimer, Profiler::lTimer;

#endif //GRAPHICS_PROFILER_HPP
