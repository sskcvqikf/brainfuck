#include "brainfuck.hh"
#include "exceptions.hh"

#include <stdexcept>
#include <stack>

namespace pd
{

brainfuck::brainfuck(const char* bf)
{
    using namespace detail;
    using uniq_op = std::unique_ptr<operation>;

    std::stack<detail::storage_operation*> storages;
    storages.push(&program);

    auto add_op = [&storages](uniq_op op)
    {
        storages.top()->push_operation(std::move(op));
    };

    auto get_simple_op = [](char c, int n)
        -> std::unique_ptr<simple_operation>
    {
        switch (c)
        {
            case '+':
                return std::make_unique<increment_byte>(n);

            case '-':
                return std::make_unique<decrement_byte>(n);

            case '>':
                return std::make_unique<increment_dataptr>(n);

            case '<':
                return std::make_unique<decrement_dataptr>(n);
            
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
            auto new_loop = std::make_unique<loop>();
            auto new_loop_ptr = new_loop.get();
            add_op(std::move(new_loop));
            storages.push(new_loop_ptr);
            continue;
        }

        if (c == '.')
            add_op(std::make_unique<output_byte>());
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
