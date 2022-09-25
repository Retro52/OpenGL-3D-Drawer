//
// Created by Anton on 22.09.2022.
//

#ifndef GRAPHICS_UNIQUEIDGENERATOR_HPP
#define GRAPHICS_UNIQUEIDGENERATOR_HPP

class UniqueIdGenerator
{
public:
    static unsigned long GetId() { return count++; }

private:
    static unsigned long count;
};

inline unsigned long UniqueIdGenerator::count = 0;

#endif //GRAPHICS_UNIQUEIDGENERATOR_HPP
