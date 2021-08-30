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

byte_t* op::get_data_ptr() { return data_ptr_; }
op* op::get_nop_() { return nop_.get(); }
std::shared_ptr<int> op::get_dp() { return dp_; }

op::~op() {}

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
    if (++*dp_ >= (size - 1))
        throw std::overflow_error("Data pointer is more than array size");
}

void ddp::execute_impl()
{
    if(--*dp_ < 0)
        throw std::overflow_error("Data pointer is less than 0");
}

void ib::execute_impl()
{
    ++*(data_ptr_ + *dp_);
}

void db::execute_impl()
{
    --*(data_ptr_ + *dp_);
}

void ob::execute_impl()
{
    std::cout << *(data_ptr_ + *dp_);
}

void el::execute()
{
    if (*(data_ptr_ + *dp_) == 0)
    {
        if (nop_ != nullptr)
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
        if (nop_ != nullptr)
            el_ptr_->execute();
    }
    else
        nop_->execute();
}

op_factory::op_factory(std::shared_ptr<int> dp, char *data)
    : dp_(std::move(dp)), data_(data) {}

std::unique_ptr<op> op_factory::get_idp() { return std::make_unique<idp>(dp_, data_); }

std::unique_ptr<op> op_factory::get_ddp() { return std::make_unique<ddp>(dp_, data_); }

std::unique_ptr<op> op_factory::get_ib() { return std::make_unique<ib>(dp_, data_); }

std::unique_ptr<op> op_factory::get_db() { return std::make_unique<db>(dp_, data_); }

std::unique_ptr<op> op_factory::get_ob() { return std::make_unique<ob>(dp_, data_); }

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

void op_factory::post_process()
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
    detail::op_factory factory(std::move(tree_.get_dp()), tree_.get_data_ptr());

    for(const char* i = bf; *i != '\0'; ++i)
    {
        char di = *i;
        switch (di)
        {
            case '>':
                last_ = last_->add_next(factory.get_idp());
                break;
            case '<':
                last_ = last_->add_next(factory.get_ddp());
                break;
            case '+':
                last_ = last_->add_next(factory.get_ib());
                break;
            case '-':
                last_ = last_->add_next(factory.get_db());
                break;
            case '.':
                last_ = last_->add_next(factory.get_ob());
                break;
            case '[':
            {
                last_ = last_->add_next(factory.get_bl());
                break;
            }
            case ']':
            {
                last_ = last_->add_next(factory.get_el());
                break;
            }
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
    factory.post_process();
}

void brainfuck::execute()
{
    tree_.execute();
}
} // namespace pd
