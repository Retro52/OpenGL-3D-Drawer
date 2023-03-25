//
// Created by Anton on 25.08.2022.
//

#include "EngineException.h"

EngineException::EngineException(std::string error) : message(std::move(error)) {}

const char *EngineException::what() const noexcept
{
    return message.c_str();
}
