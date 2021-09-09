#ifndef BRAINFUCK_OPERATIONS_HH_
#define BRAINFUCK_OPERATIONS_HH_ 
#pragma once

#include <memory>

#include "buffer.hh"

namespace pd
{

namespace detail
{

struct operation
{
    void
    execute(buffer*);

    virtual ~operation();

private:
    virtual void
    execute_impl(buffer*) = 0;
};

// ----- SIMPLE OPERAIONS -----
struct simple_operation : operation
{
    simple_operation(int n);

protected:
    int n_;
};

struct increment_dataptr final : simple_operation 
{
    using simple_operation::simple_operation;

private:
    void
    execute_impl(buffer*) override;
};

struct decrement_dataptr final : simple_operation
{
    using simple_operation::simple_operation;

private:
    void
    execute_impl(buffer*) override;
};

struct increment_byte final : simple_operation 
{
    using simple_operation::simple_operation;

private:
    void
    execute_impl(buffer*) override;
};

struct decrement_byte final : simple_operation
{
    using simple_operation::simple_operation;

private:
    void
    execute_impl(buffer*) override;
};

struct output_byte final : operation 
{
private:
    void
    execute_impl(buffer*) override;
};

// ----- OPERATIONS WITH STORAGE -----
struct storage_operation : operation
{
    void
    push_operation(operation* op);
private:
    virtual void
    push_operation_impl(operation* op) = 0;
};

struct loop final : storage_operation
{
    using storage_t = std::vector<detail::operation*>;

    ~loop() override;
    
private:
    void
    execute_impl(buffer*) override;

    void
    push_operation_impl(operation*) override;

    storage_t operations;
};

struct prog final : storage_operation
{
    using storage_t = std::vector<detail::operation*>;

    ~prog() override;

private:
    void
    execute_impl(buffer*) override;

    void
    push_operation_impl(operation*) override;

    storage_t operations;
};

} // namespace detail

} // namespace pd

#endif // BRAINFUCK_OPERATIONS_HH_
