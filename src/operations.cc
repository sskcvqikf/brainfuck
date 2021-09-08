#include <exception>
#include <iostream>
#include "operations.hh"

namespace pd
{

namespace detail
{

operation::operation(std::shared_ptr<int> dataptr, byte_t *data_ptr)
    : dataptr_(std::move(dataptr)), data_ptr_(data_ptr) {}

operation*
operation::add_next(std::unique_ptr<operation> next_operation)
{
    next_operation_ = std::move(next_operation);
    return  next_operation_.get();
}

byte_t*
operation::get_data_ptr() const noexcept { return data_ptr_; }

operation*
operation::get_next_operation_() const noexcept
{ return next_operation_.get(); }

std::shared_ptr<int>
operation::get_dataptr() const noexcept { return dataptr_; }

operation::~operation() {}

simple_operation::simple_operation(std::shared_ptr<int> dataptr,
        byte_t *data_ptr, int n)
    : base(dataptr, data_ptr), n_{n} {}

void
simple_operation::execute()
{
    execute_impl();
    if (next_operation_)
        next_operation_->execute();
}

void
brainfuck_tree::execute()
{
    if (next_operation_)
        next_operation_->execute();
}

void
icrement_dataptr::execute_impl()
{
    if (*dataptr_ + n_ >= (size - 1))
        throw std::overflow_error(
                "Data pointer is more than array size");
    *dataptr_ += n_;
}

void
decrement_dataptr::execute_impl()
{
    if (*dataptr_ - n_ < 0)
        throw std::overflow_error(
                "Data pointer is more than array size");
    *dataptr_ -= n_;
}

void
icrement_byte::execute_impl()
{ *(data_ptr_ + *dataptr_) += n_; }

void
decrement_byte::execute_impl()
{ *(data_ptr_ + *dataptr_) -= n_; }

void
output_byte::execute()
{
    std::cout << *(data_ptr_ + *dataptr_);
    if (next_operation_)
        next_operation_->execute();
}

void
end_loop::execute()
{
    if (*(data_ptr_ + *dataptr_) == 0)
    {
        if (next_operation_)
            next_operation_->execute();
    }
    else
        begin_loop_ptr_->execute();
}

void
end_loop::set_begin_loop_ptr(operation* begin_loop_ptr)
{ begin_loop_ptr_ = begin_loop_ptr; }

begin_loop::begin_loop(std::shared_ptr<int> dataptr, byte_t *data_ptr,
        operation *end_loop_ptr)
    : base(dataptr, data_ptr), end_loop_ptr_(end_loop_ptr) {}

void
begin_loop::execute()
{
    if (*(data_ptr_ + *dataptr_) == 0)
    {
        end_loop_ptr_->execute();
    }
    else
        next_operation_->execute();
}

} // namespace detail

} // namespace pd
