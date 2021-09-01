#include <iostream>
#include <algorithm>
#include "bf.hh"

namespace pd
{
namespace detail
{
op::op(std::shared_ptr<int> dp, byte_t *data_ptr)
    : dp_(std::move(dp)), data_ptr_(data_ptr) {}

op* op::add_next(std::unique_ptr<op> nop)
{
    nop_ = std::move(nop);
    return  nop_.get();
}

byte_t* op::get_data_ptr() const noexcept { return data_ptr_; }
op* op::get_nop_() const noexcept { return nop_.get(); }
std::shared_ptr<int> op::get_dp() const noexcept { return dp_; }

op::~op() {}

op_simple::op_simple(std::shared_ptr<int> dp, byte_t *data_ptr, int n)
    : base(dp, data_ptr), n_{n} {}

void op_simple::execute()
{
    execute_impl();
    if (nop_)
        nop_->execute();
}

void brainfuck_tree::execute()
{
    if (nop_)
        nop_->execute();
}

void idp::execute_impl()
{
    if (*dp_ + n_ >= (size - 1))
        throw std::overflow_error("Data pointer is more than array size");
    *dp_ += n_;
}

void ddp::execute_impl()
{
    if (*dp_ - n_ < 0)
        throw std::overflow_error("Data pointer is more than array size");
    *dp_ -= n_;
}

void ib::execute_impl()
{
    *(data_ptr_ + *dp_) += n_;
}

void db::execute_impl()
{
    *(data_ptr_ + *dp_) -= n_;
}

void ob::execute()
{
    std::cout << *(data_ptr_ + *dp_);
    if (nop_)
        nop_->execute();
}

void el::execute()
{
    if (*(data_ptr_ + *dp_) == 0)
    {
        if (nop_)
            nop_->execute();
    }
    else
        bl_ptr_->execute();
}

void el::set_bl_ptr(op* bl_ptr) { bl_ptr_ = bl_ptr; }

bl::bl(std::shared_ptr<int> dp, byte_t *data_ptr, op *el_ptr)
    : base(dp, data_ptr), el_ptr_(el_ptr) {}

void bl::execute()
{
    if (*(data_ptr_ + *dp_) == 0)
    {
        el_ptr_->execute();
    }
    else
        nop_->execute();
}

op_factory::op_factory(std::shared_ptr<int> dp, char *data)
    : dp_(std::move(dp)), data_(data) {}

std::unique_ptr<op> op_factory::get_idp(int n) const { return std::make_unique<idp>(dp_, data_, n); }

std::unique_ptr<op> op_factory::get_ddp(int n) const { return std::make_unique<ddp>(dp_, data_, n); }

std::unique_ptr<op> op_factory::get_ib(int n) const { return std::make_unique<ib>(dp_, data_, n); }

std::unique_ptr<op> op_factory::get_db(int n) const { return std::make_unique<db>(dp_, data_, n); }

std::unique_ptr<op> op_factory::get_ob() const { return std::make_unique<ob>(dp_, data_); }

std::unique_ptr<op> op_factory::get_bl()
{ 
    auto el_ptr = std::make_unique<detail::el>(dp_, data_);
    auto rtn = std::make_unique<detail::bl>(dp_, data_, el_ptr.get());
    el_ptr->set_bl_ptr(rtn.get());
    stack_el.push(std::move(el_ptr));
    return rtn;
}

std::unique_ptr<op> op_factory::get_el()
{
    if (stack_el.empty())
        throw bad_brainfuck_string("Unmatched brackets appeared");
    auto rtn(std::move(stack_el.top()));
    stack_el.pop();

    return rtn;
}

void op_factory::post_process() const
{
    if (!stack_el.empty())
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
    detail::op_factory factory(tree_.get_dp(), tree_.get_data_ptr());
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
                last_ = last_->add_next(factory.get_ob());
                break;

            case '[':
                add(factory, prev_op, current_op_count);
                prev_op = 0;
                current_op_count = 1;
                last_ = last_->add_next(factory.get_bl());
                break;

            case ']':
                add(factory, prev_op, current_op_count);
                prev_op = 0;
                current_op_count = 1;
                last_ = last_->add_next(factory.get_el());
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

void brainfuck::add(const detail::op_factory &factory, char to_add, int n)
{
    switch (to_add)
    {
        case '>':
            last_ = last_->add_next(factory.get_idp(n));
            break;

        case '<':
            last_ = last_->add_next(factory.get_ddp(n));
            break;

        case '+':
            last_ = last_->add_next(factory.get_ib(n));
            break;

        case '-':
            last_ = last_->add_next(factory.get_db(n));
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
