#include <vector>
#include <stdexcept>

#include <iostream>

#include "buffer.hh"

namespace pd
{

void buffer::increment_dataptr(int n)
{
    return increment_dataptr_impl(n);
}

void buffer::decrement_dataptr(int n)
{
    return decrement_dataptr_impl(n);
}

void buffer::increment_byte(int n)
{
    return increment_byte_impl(n);
}

void buffer::decrement_byte(int n)
{
    return decrement_byte_impl(n);
}

byte_t buffer::get_byte()
{
    return get_byte_impl();
}

buffer::~buffer(){}

vector_buffer::vector_buffer()
    : idx_(0) { vec_.push_back(0); }

void vector_buffer::increment_byte_impl(int n) { vec_.at(idx_) += n; }

void vector_buffer::decrement_byte_impl(int n) { vec_.at(idx_) -= n; }

void vector_buffer::increment_dataptr_impl(int n)
{
    auto tmp = idx_ + n;
    if (tmp > 30000)
        throw std::overflow_error(
                "Data pointer is more than 30000;");
    tmp -= vec_.size() - 1;
    if (tmp > 0)
        add_n(tmp);
    idx_ += n;
}

void vector_buffer::decrement_dataptr_impl(int n)
{
    auto tmp = idx_ - n;
    if (tmp < 0)
        throw std::underflow_error(
                "Data pointer is less than 0;");
    idx_ = tmp;
}

byte_t vector_buffer::get_byte_impl() { return vec_.at(idx_); }

void vector_buffer::add_n(int n)
{
    while (n--)
    {
        vec_.push_back(0);
    }
}

} // namespace pd
