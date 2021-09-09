#ifndef BRAINFUCK_BRAINFUCK_HH_
#define BRAINFUCK_BRAINFUCK_HH_ 
#pragma once

#include <memory>

#include "operations.hh"
#include "buffer.hh"

namespace pd
{

struct brainfuck final
{
    brainfuck(const char* bf);

    void
    execute(std::unique_ptr<buffer>);

private:
    detail::prog program;
};

} // namespace pd

#endif // BRAINFUCK_BRAINFUCK_HH_
