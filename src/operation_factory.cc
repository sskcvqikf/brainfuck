#include "operation_factory.hh"
#include "exceptions.hh"

namespace pd
{
namespace detail
{

operation_factory::operation_factory(std::shared_ptr<int> dataptr,
        char *data)
    : dataptr_(std::move(dataptr)), data_(data) {}

std::unique_ptr<operation>
operation_factory::get_icrement_dataptr(int n) const
{ return std::make_unique<icrement_dataptr>(dataptr_, data_, n); }

std::unique_ptr<operation>
operation_factory::get_decrement_dataptr(int n) const
{ return std::make_unique<decrement_dataptr>(dataptr_, data_, n); }

std::unique_ptr<operation>
operation_factory::get_icrement_byte(int n) const
{ return std::make_unique<icrement_byte>(dataptr_, data_, n); }

std::unique_ptr<operation>
operation_factory::get_decrement_byte(int n) const
{ return std::make_unique<decrement_byte>(dataptr_, data_, n); }

std::unique_ptr<operation>
operation_factory::get_output_byte() const
{ return std::make_unique<output_byte>(dataptr_, data_); }

std::unique_ptr<operation>
operation_factory::get_begin_loop()
{ 
    auto end_loop_ptr = std::make_unique<detail::end_loop>(dataptr_,
            data_);
    auto rtn = std::make_unique<detail::begin_loop>(dataptr_, data_,
            end_loop_ptr.get());
    end_loop_ptr->set_begin_loop_ptr(rtn.get());
    stack_end_loop.push(std::move(end_loop_ptr));
    return rtn;
}

std::unique_ptr<operation>
operation_factory::get_end_loop()
{
    if (stack_end_loop.empty())
        throw bad_brainfuck_string("Unmatched brackets appeared");
    auto rtn(std::move(stack_end_loop.top()));
    stack_end_loop.pop();

    return rtn;
}

void
operation_factory::post_process() const
{
    if (!stack_end_loop.empty())
        throw bad_brainfuck_string("Unmatched brackets appeared");
}

} // namespace detail
} // namespace pd
