#ifndef BRAINFUCK_BUFFER_HH_
#define BRAINFUCK_BUFFER_HH_ 
#pragma once

#include <vector>

#include "cfg.hh"

namespace pd
{

struct buffer
{
    void
    increment_dataptr(int n=1);

    void
    decrement_dataptr(int n=1);

    void
    increment_byte(int n=1);

    void
    decrement_byte(int n=1);

    byte_t get_byte();

    virtual ~buffer();

private:
    virtual void
    increment_dataptr_impl(int n) = 0;

    virtual void
    decrement_dataptr_impl(int n) = 0;

    virtual void
    increment_byte_impl(int n) = 0;

    virtual void
    decrement_byte_impl(int n) = 0;

    virtual byte_t
    get_byte_impl() = 0;
};

struct vector_buffer : buffer
{
    vector_buffer();

private:
    void
    increment_byte_impl(int n) override;

    void
    decrement_byte_impl(int n) override;

    void
    increment_dataptr_impl(int n) override;

    void
    decrement_dataptr_impl(int n) override;

    byte_t
    get_byte_impl() override;

    void
    add_n(int n);

    std::vector<byte_t> vec_;
    int idx_;
};


struct char_buffer : buffer
{
    char_buffer();

private:
    void
    increment_byte_impl(int n) override;

    void
    decrement_byte_impl(int n) override;

    void
    increment_dataptr_impl(int n) override;

    void
    decrement_dataptr_impl(int n) override;

    byte_t
    get_byte_impl() override;

    byte_t ary_[30000] = {0};
    int idx_;
};

} // namespace pd

#endif // BRAINFUCK_BUFFER_HH_
