#ifndef BRAINFUCK_BF_HH_
#define BRAINFUCK_BF_HH_ 
#pragma once

#include <memory>
#include <exception>
#include <stack>

namespace pd
{
using byte_t = char;
static constexpr int size = 30000;

struct bad_brainfuck_string : std::exception
{
    bad_brainfuck_string(const char *message);

    const char* what() const noexcept;

private:
    const char *message_;
};

namespace detail
{
struct brainfuck_tree;

struct operation
{
    operation(std::shared_ptr<int> dataptr, byte_t *data_ptr);

    virtual void execute() = 0;

    virtual operation* add_next(std::unique_ptr<operation> next_operation);

    byte_t* get_data_ptr() const noexcept;
    operation* get_next_operation_() const noexcept;
    std::shared_ptr<int> get_dataptr() const noexcept;
    
    virtual ~operation();

protected:
    std::unique_ptr<operation> next_operation_; // next instruction
    std::shared_ptr<int> dataptr_; // data ptr
    byte_t *data_ptr_;
};

struct simple_operation : operation
{
    using base = operation;

    simple_operation(std::shared_ptr<int> dataptr, byte_t *data_ptr, int n);

    virtual void execute();
protected:
    int n_;
private:
    virtual void execute_impl() = 0;
};


struct brainfuck_tree final : operation
{
    using operation::operation;
    void execute() override;
};

struct icrement_dataptr final : simple_operation // increment data pointer
{
    using simple_operation::simple_operation;

private:
    void execute_impl() override;
};

struct decrement_dataptr final : simple_operation // decrement data pointer
{
    using simple_operation::simple_operation;

private:
    void execute_impl() override;
};

struct icrement_byte final : simple_operation // increment byte in current cend_loopl
{
    using simple_operation::simple_operation;

private:
    void execute_impl() override;
};

struct decrement_byte final : simple_operation // decrement byte in current cend_loopl
{
    using simple_operation::simple_operation;

private:
    void execute_impl() override;
};

struct output_byte final : operation // output byte in current cend_loopl
{
    using operation::operation;

    void execute() override;
};

struct end_loop final : operation // end loop
{
    using operation::operation;
    
    void execute() override;

    void set_begin_loop_ptr(operation* begin_loop_ptr);
private:
    operation* begin_loop_ptr_ = nullptr;
};

struct begin_loop final : operation // begin loop
{
    using base = operation;
    
    begin_loop(std::shared_ptr<int> dataptr, byte_t *data_ptr, operation *end_loop_ptr);

    void execute() override;

private:
    operation* end_loop_ptr_;
};

struct operation_factory final
{
    operation_factory(std::shared_ptr<int> dataptr, char *data);

    std::unique_ptr<operation> get_icrement_dataptr(int) const;

    std::unique_ptr<operation> get_decrement_dataptr(int) const;
    
    std::unique_ptr<operation> get_icrement_byte(int) const;

    std::unique_ptr<operation> get_decrement_byte(int) const;

    std::unique_ptr<operation> get_output_byte() const;

    std::unique_ptr<operation> get_begin_loop();

    std::unique_ptr<operation> get_end_loop();

    void post_process() const;

private:
    std::stack<std::unique_ptr<end_loop>> stack_end_loop;
    std::shared_ptr<int> dataptr_;
    char* data_;
};

} // namespace detail

struct brainfuck final
{
    brainfuck(const char* bf);

    void execute();

    void add(const detail::operation_factory &factory, char, int);

private:
    detail::brainfuck_tree tree_;
    detail::operation *last_;
    byte_t data_[size] = {0};
};
} // namespace pd
#endif // BRAINFUCK_BF_HH_
