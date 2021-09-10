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


void storage_operation::push_operation(uniq_op op)
{ push_operation_impl(std::move(op)); }

void
loop::push_operation_impl(uniq_op op)
{ operations.push_back(std::move(op)); }

void
loop::execute_impl(buffer *buff) const
{
    while (buff->get_byte() != 0)
        std::for_each(operations.cbegin(), operations.cend(),
                [buff](auto& i) { i->execute(buff); });
}

void
prog::push_operation_impl(uniq_op op)
{ operations.push_back(std::move(op)); }

void
prog::execute_impl(buffer *buff) const
{
    std::for_each(operations.cbegin(), operations.cend(),
            [buff](auto& i) { i->execute(buff); });
}

} // namespace detail

} // namespace pd
