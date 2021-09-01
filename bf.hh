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

struct op
{
    op(std::shared_ptr<int> dp, byte_t *data_ptr);

    virtual void execute() = 0;

    virtual op* add_next(std::unique_ptr<op> nop);

    byte_t* get_data_ptr() const noexcept;
    op* get_nop_() const noexcept;
    std::shared_ptr<int> get_dp() const noexcept;
    
    virtual ~op();

protected:
    std::unique_ptr<op> nop_; // next instruction
    std::shared_ptr<int> dp_; // data ptr
    byte_t *data_ptr_;
};

struct op_simple : op
{
    using base = op;

    op_simple(std::shared_ptr<int> dp, byte_t *data_ptr, int n);

    virtual void execute();
protected:
    int n_;
private:
    virtual void execute_impl() = 0;
};


struct brainfuck_tree final : op
{
    using op::op;
    void execute() override;
};

struct idp final : op_simple // increment data pointer
{
    using op_simple::op_simple;

private:
    void execute_impl() override;
};

struct ddp final : op_simple // decrement data pointer
{
    using op_simple::op_simple;

private:
    void execute_impl() override;
};

struct ib final : op_simple // increment byte in current cell
{
    using op_simple::op_simple;

private:
    void execute_impl() override;
};

struct db final : op_simple // decrement byte in current cell
{
    using op_simple::op_simple;

private:
    void execute_impl() override;
};

struct ob final : op // output byte in current cell
{
    using op::op;

    void execute() override;
};

struct el final : op // end loop
{
    using op::op;
    
    void execute() override;

    void set_bl_ptr(op* bl_ptr);
private:
    op* bl_ptr_ = nullptr;
};

struct bl final : op // begin loop
{
    using base = op;
    
    bl(std::shared_ptr<int> dp, byte_t *data_ptr, op *el_ptr);

    void execute() override;

private:
    op* el_ptr_;
};

struct op_factory final
{
    op_factory(std::shared_ptr<int> dp, char *data);

    std::unique_ptr<op> get_idp(int) const;

    std::unique_ptr<op> get_ddp(int) const;
    
    std::unique_ptr<op> get_ib(int) const;

    std::unique_ptr<op> get_db(int) const;

    std::unique_ptr<op> get_ob() const;

    std::unique_ptr<op> get_bl();

    std::unique_ptr<op> get_el();

    void post_process() const;

private:
    std::stack<std::unique_ptr<el>> stack_el;
    std::shared_ptr<int> dp_;
    char* data_;
};

} // namespace detail

struct brainfuck final
{
    brainfuck(const char* bf);

    void execute();

    void add(const detail::op_factory &factory, char, int);

private:
    detail::brainfuck_tree tree_;
    detail::op *last_;
    byte_t data_[size] = {0};
};
} // namespace pd
#endif // BRAINFUCK_BF_HH_
