#include "brainfuck.hh"
#include "exceptions.hh"

#include <stdexcept>
#include <stack>

namespace pd
{

brainfuck::brainfuck(const char* bf)
{
    using namespace detail;

    std::stack<detail::storage_operation*> storages;
    storages.push(&program);

    auto add_op = [&storages](operation *op)
    {
        storages.top()->push_operation(op);
    };

    auto get_simple_op = [](char c, int n) -> simple_operation*
    {
        switch (c)
        {
            case '+':
                return new increment_byte(n);

            case '-':
                return new decrement_byte(n);

            case '>':
                return new increment_dataptr(n);

            case '<':
                return new decrement_dataptr(n);
            
            default:
                throw std::runtime_error(
                        "Something strange is going on");
        }
    };

    int curr_op_count = 1;
    char curr_op = 0;

    for(auto cp = bf; *cp != 0; ++cp)
    {
        auto c = *cp;
        if (c == '+' || c == '-' ||
            c == '>' || c == '<')
        {
            if (c == curr_op)
                curr_op_count += 1;
            else
            {
                if (curr_op != 0)
                    add_op(get_simple_op(curr_op, curr_op_count));
                curr_op = c;
                curr_op_count = 1;
            }
            continue;
        }

        if (c == '\t' || c == '\n' || c == ' ')
            continue;

        if (c != ']' && c != '[' && c != '.')
            throw bad_brainfuck_string(
                    "Unknown symbol appeared");

        if (curr_op != 0)
        {
            add_op(get_simple_op(curr_op, curr_op_count));
            curr_op = 0;
            curr_op_count = 1;
        }

        if (c == ']')
        {
            if (storages.size() == 1)
                throw bad_brainfuck_string(
                        "Unmatched brackets appeared");
            storages.pop();
            continue;
        }
        
        if (c == '[')
        {
            loop *new_loop = new loop();
            add_op(new_loop);
            storages.push(new_loop);
            continue;
        }

        if (c == '.')
            add_op(new output_byte());
    }
    if (curr_op != 0)
        add_op(get_simple_op(curr_op, curr_op_count));

    if (storages.size() != 1)
        throw bad_brainfuck_string(
                "Unmatched brackets appeared");
}

void
brainfuck::execute(std::unique_ptr<buffer> buff)
{
    program.execute(buff.get());
}

} // namespace pd
