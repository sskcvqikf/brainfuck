#include <algorithm>
#include <iostream>
#include "operations.hh"

namespace pd
{

namespace detail
{

operation::~operation() {}

void
operation::execute(buffer *buff) const
{ execute_impl(buff); }

simple_operation::simple_operation(int n)
    : n_{n} {}

void
increment_dataptr::execute_impl(buffer *buff) const
{ buff->increment_dataptr(n_); }

void
decrement_dataptr::execute_impl(buffer *buff) const
{ buff->decrement_dataptr(n_); }

void
increment_byte::execute_impl(buffer *buff) const
{ buff->increment_byte(n_); }
    
void
decrement_byte::execute_impl(buffer *buff) const
{ buff->decrement_byte(n_); }

void
output_byte::execute_impl(buffer *buff) const
{ std::cout << buff->get_byte(); }


void storage_operation::push_operation(operation *op)
{ push_operation_impl(op); }

void
loop::push_operation_impl(operation *op)
{ operations.push_back(op); }

void
loop::execute_impl(buffer *buff) const
{
    while (buff->get_byte() != 0)
    {
        std::for_each(operations.begin(), operations.end(),
                [buff](auto i) { i->execute(buff); });
    }
}

loop::~loop()
{
    std::for_each(operations.cbegin(), operations.cend(),
            [](auto i) { delete i; });
    operations.clear();
}

void prog::push_operation_impl(operation *op)
{ operations.push_back(op); }

void prog::execute_impl(buffer *buff) const
{
    std::for_each(operations.cbegin(), operations.cend(),
            [buff](auto i) { i->execute(buff); });
}

prog::~prog()
{
    std::for_each(operations.cbegin(), operations.cend(),
            [](auto i) { delete i; });
    operations.clear();
}

} // namespace detail

} // namespace pd
