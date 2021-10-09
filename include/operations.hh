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
    execute(buffer*) const;

    virtual ~operation();

private:
    virtual void
    execute_impl(buffer*) const = 0;
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
    execute_impl(buffer*) const override;
};

struct decrement_dataptr final : simple_operation
{
    using simple_operation::simple_operation;

private:
    void
    execute_impl(buffer*) const override;
};

struct increment_byte final : simple_operation 
{
    using simple_operation::simple_operation;

private:
    void
    execute_impl(buffer*) const override;
};

struct decrement_byte final : simple_operation
{
    using simple_operation::simple_operation;

private:
    void
    execute_impl(buffer*) const override;
};

struct output_byte final : operation 
{
private:
    void
    execute_impl(buffer*) const override;
};

// ----- OPERATIONS WITH STORAGE -----
struct storage_operation : operation
{
    using uniq_op = std::unique_ptr<detail::operation>;

    void
    push_operation(uniq_op);

private:
    virtual void
    push_operation_impl(uniq_op) = 0;
};

struct loop final : storage_operation
{
    using storage_operation::uniq_op;
    using storage_t = std::vector<uniq_op>;

private:
    void
    execute_impl(buffer*) const override;

    void
    push_operation_impl(uniq_op) override;

    storage_t operations;
};

struct prog final : storage_operation
{
    using storage_operation::uniq_op;
    using storage_t = std::vector<uniq_op>;

private:
    void
    execute_impl(buffer*) const override;

    void
    push_operation_impl(uniq_op) override;

    storage_t operations;
};

} // namespace detail

} // namespace pd

#endif // BRAINFUCK_OPERATIONS_HH_
