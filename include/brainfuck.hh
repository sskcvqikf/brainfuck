#ifndef BRAINFUCK_BRAINFUCK_HH_
#define BRAINFUCK_BRAINFUCK_HH_ 
#pragma once

#include "operations.hh"
#include "operation_factory.hh"

namespace pd
{

struct brainfuck final
{
    brainfuck(const char* bf);

    void
    execute();

    void
    add(const detail::operation_factory &factory, char, int);

private:
    detail::brainfuck_tree tree_;
    detail::operation *last_;
    byte_t data_[size] = {0};
};

} // namespace pd

#endif // BRAINFUCK_BRAINFUCK_HH_
