//
// Created by Anton on 25.08.2022.
//

#ifndef GRAPHICS_INGAMEEXCEPTION_H
#define GRAPHICS_INGAMEEXCEPTION_H
#include <string>
#include <utility>


class InGameException : public std::exception
{
public:

    /**
     * Creates exception
     * @param error error message
     */
    explicit InGameException(std::string error);

    /**
     * Get error message
     * @return c-style const char array containing error message
     */
    [[nodiscard]] const char * what() const noexcept override;

private:
    std::string message;
};

constexpr void GAME_ASSERT(bool var, const std::string& message) { if (!var) { throw InGameException(message); } }

#endif //GRAPHICS_INGAMEEXCEPTION_H
