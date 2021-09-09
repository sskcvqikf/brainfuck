#ifndef BRAINFUCK_EXCEPTIONS_HH_
#define BRAINFUCK_EXCEPTIONS_HH_ 
#pragma once

#include <exception>

namespace pd
{

struct bad_brainfuck_string : std::exception
{
    bad_brainfuck_string(const char *message);

    const char*
    what() const noexcept;

private:
    const char *message_;
};

} // namespace pd

#endif // BRAINFUCK_EXCEPTIONS_HH_
