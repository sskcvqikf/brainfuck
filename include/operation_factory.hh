#ifndef BRAINFUCK_OPERATION_FACTORY_HH_
#define BRAINFUCK_OPERATION_FACTORY_HH_ 
#pragma once

#include <stack>

#include "operations.hh"

namespace pd
{

namespace detail
{
struct operation_factory final
{
    operation_factory(std::shared_ptr<int> dataptr, char *data);

    std::unique_ptr<operation>
    get_icrement_dataptr(int) const;

    std::unique_ptr<operation>
    get_decrement_dataptr(int) const;
    
    std::unique_ptr<operation>
    get_icrement_byte(int) const;

    std::unique_ptr<operation>
    get_decrement_byte(int) const;

    std::unique_ptr<operation>
    get_output_byte() const;

    std::unique_ptr<operation>
    get_begin_loop();

    std::unique_ptr<operation>
    get_end_loop();

    void post_process() const;

private:
    std::stack<std::unique_ptr<end_loop>> stack_end_loop;
    std::shared_ptr<int> dataptr_;
    char* data_;
};
} // namespace detail
} // namespace pd

#endif // BRAINFUCK_OPERATION_FACTORY_HH_
