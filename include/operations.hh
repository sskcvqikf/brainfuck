#ifndef BRAINFUCK_OPERATIONS_HH_
#define BRAINFUCK_OPERATIONS_HH_ 
#pragma once

#include <memory>

#include "cfg.hh"

namespace pd
{

namespace detail
{

struct operation
{
    operation(std::shared_ptr<int> dataptr, byte_t *data_ptr);

    virtual void
    execute() = 0;

    virtual operation*
    add_next(std::unique_ptr<operation> next_operation);

    byte_t*
    get_data_ptr() const noexcept;

    operation*
    get_next_operation_() const noexcept;

    std::shared_ptr<int>
    get_dataptr() const noexcept;
    
    virtual ~operation();

protected:
    std::unique_ptr<operation> next_operation_; 
    std::shared_ptr<int> dataptr_; 
    byte_t *data_ptr_;
};

struct simple_operation : operation
{
    using base = operation;

    simple_operation(std::shared_ptr<int> dataptr, byte_t *data_ptr,
            int n);

    virtual void
    execute();

protected:
    int n_;

private:
    virtual void
    execute_impl() = 0;
};


struct brainfuck_tree final : operation
{
    using operation::operation;

    void
    execute() override;
};

struct icrement_dataptr final : simple_operation 
{
    using simple_operation::simple_operation;

private:
    void
    execute_impl() override;
};

struct decrement_dataptr final : simple_operation
{
    using simple_operation::simple_operation;

private:
    void
    execute_impl() override;
};

struct icrement_byte final : simple_operation 
{
    using simple_operation::simple_operation;

private:
    void
    execute_impl() override;
};

struct decrement_byte final : simple_operation
{
    using simple_operation::simple_operation;

private:
    void
    execute_impl() override;
};

struct output_byte final : operation 
{
    using operation::operation;

    void
    execute() override;
};

struct end_loop final : operation 
{
    using operation::operation;
    
    void
    execute() override;

    void
    set_begin_loop_ptr(operation* begin_loop_ptr);

private:
    operation* begin_loop_ptr_ = nullptr;
};

struct begin_loop final : operation
{
    using base = operation;
    
    begin_loop(std::shared_ptr<int> dataptr, byte_t *data_ptr,
            operation *end_loop_ptr);

    void
    execute() override;

private:
    operation* end_loop_ptr_;
};

} // namespace detail

} // namespace pd

#endif // BRAINFUCK_OPERATIONS_HH_
