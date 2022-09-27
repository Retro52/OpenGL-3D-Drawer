//
// Created by Anton on 22.09.2022.
//

#ifndef GRAPHICS_UNIQUEID_HPP
#define GRAPHICS_UNIQUEID_HPP

#include "UniqueIdGenerator.hpp"

class UniqueID
{
public:
    UniqueID() : id(UniqueIdGenerator::GetId()) {};
    explicit UniqueID(unsigned long id) : id(id) {};
    bool operator == (const UniqueID& other) const { return other.id == id; }

    explicit operator unsigned long () const { return id; }
private:
    unsigned long id;
};

#endif //GRAPHICS_UNIQUEID_HPP
