#include <iostream>
#include <algorithm>
#include "bf.hh"

namespace pd
{
namespace detail
{
operation::operation(std::shared_ptr<int> dataptr, byte_t *data_ptr)
    : dataptr_(std::move(dataptr)), data_ptr_(data_ptr) {}

operation* operation::add_next(std::unique_ptr<operation> next_operation)
{
    next_operation_ = std::move(next_operation);
    return  next_operation_.get();
}

byte_t* operation::get_data_ptr() const noexcept { return data_ptr_; }
operation* operation::get_next_operation_() const noexcept { return next_operation_.get(); }
std::shared_ptr<int> operation::get_dataptr() const noexcept { return dataptr_; }

operation::~operation() {}

simple_operation::simple_operation(std::shared_ptr<int> dataptr, byte_t *data_ptr, int n)
    : base(dataptr, data_ptr), n_{n} {}

void simple_operation::execute()
{
    execute_impl();
    if (next_operation_)
        next_operation_->execute();
}

void brainfuck_tree::execute()
{
    if (next_operation_)
        next_operation_->execute();
}

void icrement_dataptr::execute_impl()
{
    if (*dataptr_ + n_ >= (size - 1))
        throw std::overflow_error("Data pointer is more than array size");
    *dataptr_ += n_;
}

void decrement_dataptr::execute_impl()
{
    if (*dataptr_ - n_ < 0)
        throw std::overflow_error("Data pointer is more than array size");
    *dataptr_ -= n_;
}

void icrement_byte::execute_impl()
{
    *(data_ptr_ + *dataptr_) += n_;
}

void decrement_byte::execute_impl()
{
    *(data_ptr_ + *dataptr_) -= n_;
}

void output_byte::execute()
{
    std::cout << *(data_ptr_ + *dataptr_);
    if (next_operation_)
        next_operation_->execute();
}

void end_loop::execute()
{
    if (*(data_ptr_ + *dataptr_) == 0)
    {
        if (next_operation_)
            next_operation_->execute();
    }
    else
        begin_loop_ptr_->execute();
}

void end_loop::set_begin_loop_ptr(operation* begin_loop_ptr) { begin_loop_ptr_ = begin_loop_ptr; }

begin_loop::begin_loop(std::shared_ptr<int> dataptr, byte_t *data_ptr, operation *end_loop_ptr)
    : base(dataptr, data_ptr), end_loop_ptr_(end_loop_ptr) {}

void begin_loop::execute()
{
    if (*(data_ptr_ + *dataptr_) == 0)
    {
        end_loop_ptr_->execute();
    }
    else
        next_operation_->execute();
}

operation_factory::operation_factory(std::shared_ptr<int> dataptr, char *data)
    : dataptr_(std::move(dataptr)), data_(data) {}

std::unique_ptr<operation> operation_factory::get_icrement_dataptr(int n) const { return std::make_unique<icrement_dataptr>(dataptr_, data_, n); }

std::unique_ptr<operation> operation_factory::get_decrement_dataptr(int n) const { return std::make_unique<decrement_dataptr>(dataptr_, data_, n); }

std::unique_ptr<operation> operation_factory::get_icrement_byte(int n) const { return std::make_unique<icrement_byte>(dataptr_, data_, n); }

std::unique_ptr<operation> operation_factory::get_decrement_byte(int n) const { return std::make_unique<decrement_byte>(dataptr_, data_, n); }

std::unique_ptr<operation> operation_factory::get_output_byte() const { return std::make_unique<output_byte>(dataptr_, data_); }

std::unique_ptr<operation> operation_factory::get_begin_loop()
{ 
    auto end_loop_ptr = std::make_unique<detail::end_loop>(dataptr_, data_);
    auto rtn = std::make_unique<detail::begin_loop>(dataptr_, data_, end_loop_ptr.get());
    end_loop_ptr->set_begin_loop_ptr(rtn.get());
    stack_end_loop.push(std::move(end_loop_ptr));
    return rtn;
}

std::unique_ptr<operation> operation_factory::get_end_loop()
{
    if (stack_end_loop.empty())
        throw bad_brainfuck_string("Unmatched brackets appeared");
    auto rtn(std::move(stack_end_loop.top()));
    stack_end_loop.pop();

    return rtn;
}

void operation_factory::post_process() const
{
    if (!stack_end_loop.empty())
        throw bad_brainfuck_string("Unmatched brackets appeared");
}


} // namespace detail

bad_brainfuck_string::bad_brainfuck_string(const char *message)
    : message_(message) {}

const char* bad_brainfuck_string::what() const noexcept
{
    return message_;
}

brainfuck::brainfuck(const char* bf)
    : tree_(std::make_shared<int>(0), data_), last_(&tree_)
{
    detail::operation_factory factory(tree_.get_dataptr(), tree_.get_data_ptr());
    char prev_op = 0;
    int current_op_count = 1;
    for(const char *i = bf; *i != '\0'; ++i)
    {
        char di = *i;
        if (di == '>' || di == '<' || di == '+' || di == '-')
        {
            if (di == prev_op)
                ++current_op_count;
            else
            {
                add(factory, prev_op, current_op_count);
                prev_op = di;
                current_op_count = 1;
            }
            continue;
        }
        switch (di)
        {
            case '.':
                add(factory, prev_op, current_op_count);
                prev_op = 0;
                current_op_count = 1;
                last_ = last_->add_next(factory.get_output_byte());
                break;

            case '[':
                add(factory, prev_op, current_op_count);
                prev_op = 0;
                current_op_count = 1;
                last_ = last_->add_next(factory.get_begin_loop());
                break;

            case ']':
                add(factory, prev_op, current_op_count);
                prev_op = 0;
                current_op_count = 1;
                last_ = last_->add_next(factory.get_end_loop());
                break;

            case '\n':
                break;

            case '\t':
                break;

            case ' ':
                break;

            default:
                throw bad_brainfuck_string("Unknown symbol appeared");
        }
    }
    add(factory, prev_op, current_op_count);
    factory.post_process();
}

void brainfuck::add(const detail::operation_factory &factory, char to_add, int n)
{
    switch (to_add)
    {
        case '>':
            last_ = last_->add_next(factory.get_icrement_dataptr(n));
            break;

        case '<':
            last_ = last_->add_next(factory.get_decrement_dataptr(n));
            break;

        case '+':
            last_ = last_->add_next(factory.get_icrement_byte(n));
            break;

        case '-':
            last_ = last_->add_next(factory.get_decrement_byte(n));
            break;
        
        case 0:
            break;
    }
}

void brainfuck::execute()
{
    tree_.execute();
}
} // namespace pd
