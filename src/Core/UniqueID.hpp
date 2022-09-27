//
// Created by Anton on 22.09.2022.
//

#ifndef GRAPHICS_UNIQUEID_HPP
#define GRAPHICS_UNIQUEID_HPP

#include "UniqueIdGenerator.hpp"

class UniqueID
{
public:
    /* Default unique id generator */
    UniqueID() : id(UniqueIdGenerator::GetId()) {};

    /* Constructor from pre-created id */
    explicit UniqueID(unsigned long id) : id(id) {};

    /* Equality operator */
    bool operator == (const UniqueID& other) const { return other.id == id; }

    /* Conversion from UniqueID class to unsigned long (UniqueID.id type) operator  */
    explicit operator unsigned long () const { return id; }
private:
    /* Assigned id */
    unsigned long id;
};

#endif //GRAPHICS_UNIQUEID_HPP
