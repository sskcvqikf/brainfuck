#include "brainfuck.hh"
#include "exceptions.hh"

namespace pd
{

brainfuck::brainfuck(const char* bf)
    : tree_(std::make_shared<int>(0), data_), last_(&tree_)
{
    detail::operation_factory factory(tree_.get_dataptr(),
            tree_.get_data_ptr());
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

void
brainfuck::add(const detail::operation_factory &factory, char to_add,
        int n)
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

void
brainfuck::execute()
{
    tree_.execute();
}

} // namespace pd
