#include <iostream>
#include <memory>
#include <exception>
#include <algorithm>
#include <stack>

using byte_t = char;
static constexpr int size = 30000;

namespace detail
{
struct brainfuck_tree;

struct op
{
    op(std::shared_ptr<int> dp, byte_t *data_ptr)
        : dp_(std::move(dp)), data_ptr_(data_ptr) {}

    virtual void execute() = 0;

    virtual op* add_next(std::unique_ptr<op> nop)
    {
        nop_ = std::move(nop);
        return  nop_.get();
    }

    byte_t* get_data_ptr() { return data_ptr_; }
    op* get_nop_() { return nop_.get(); }
    std::shared_ptr<int> get_dp() { return dp_; }
    
    virtual ~op() {}

protected:
    std::unique_ptr<op> nop_; // next instruction
    std::shared_ptr<int> dp_; // data ptr
    byte_t *data_ptr_;
};

struct op_simple : op
{
    using op::op;
    virtual void execute()
    {
        execute_impl();
        if (nop_.get() != nullptr)
            nop_.get()->execute();
    }
private:
    virtual void execute_impl() = 0;
};


struct brainfuck_tree final : op_simple 
{
    using op_simple::op_simple;
    void prepare() {}
private:
    void execute_impl() override{}
};

struct idp final : op_simple // increment data pointer
{
    using op_simple::op_simple;

private:
    void execute_impl() override
    {
        if (++*dp_ >= (size - 1))
            throw std::overflow_error("Data pointer is more than array size");
    }
};

struct ddp final : op_simple // decrement data pointer
{
    using op_simple::op_simple;

private:
    void execute_impl() override
    {
        if(--*dp_ < 0)
            throw std::overflow_error("Data pointer is less than 0");
    }
};

struct id final : op_simple // increment byte in current cell
{
    using op_simple::op_simple;

private:
    void execute_impl() override
    {
        ++*(data_ptr_ + *dp_);
    }
};

struct dd final : op_simple // decrement byte in current cell
{
    using op_simple::op_simple;

private:
    void execute_impl() override
    {
        --*(data_ptr_ + *dp_);
    }
};

struct od final : op_simple // output byte in current cell
{
    using op_simple::op_simple;

private:
    void execute_impl() override
    {
        std::cout << *(data_ptr_ + *dp_);
    }
};

struct el final : op // end loop
{
    using op::op;
    
    void execute() override
    {
        if (*(data_ptr_ + *dp_) == 0)
        {
            if (nop_ != nullptr)
                nop_->execute();
        }
        else
            bl_ptr_->execute();
    }

    void set_bl_ptr(op* bl_ptr) { bl_ptr_ = bl_ptr; }
private:
    op* bl_ptr_ = nullptr;
};

struct bl final : op // begin loop
{
    using base = op;
    
    bl(std::shared_ptr<int> dp, byte_t *data_ptr, op *el_ptr)
        : base(dp, data_ptr), el_ptr_(el_ptr) {}

    void execute() override
    {
        if (*(data_ptr_ + *dp_) == 0)
        {
            if (nop_ != nullptr)
                el_ptr_->execute();
        }
        else
            nop_->execute();
    }

private:
    op* el_ptr_;
};

} // namespace detail

struct bad_brainfuck_string : std::exception
{
    bad_brainfuck_string(const char *message)
        : message_(message) {}

    const char* what() const noexcept
    {
        return message_;
    }

private:
    const char *message_;
};

struct brainfuck final
{
    brainfuck(const char* bf)
        : tree_(std::make_shared<int>(0), data_), last_(&tree_)
    {
        std::stack<std::unique_ptr<detail::el>> stack_el;
        for(const char* i = bf; *i != '\0'; ++i)
        {
            char di = *i;
            switch (di)
            {
                case '>':
                    last_ = last_->add_next(std::make_unique<detail::idp>(std::move(tree_.get_dp()),
                        tree_.get_data_ptr()));
                    break;
                case '<':
                    last_ = last_->add_next(std::make_unique<detail::ddp>(std::move(tree_.get_dp()),
                        tree_.get_data_ptr()));
                    break;
                case '+':
                    last_ = last_->add_next(std::make_unique<detail::id>(std::move(tree_.get_dp()),
                        tree_.get_data_ptr()));
                    break;
                case '-':
                    last_ = last_->add_next(std::make_unique<detail::dd>(std::move(tree_.get_dp()),
                        tree_.get_data_ptr()));
                    break;
                case '.':
                    last_ = last_->add_next(std::make_unique<detail::od>(std::move(tree_.get_dp()),
                        tree_.get_data_ptr()));
                    break;
                case '[':
                {
                    auto el_ptr = std::make_unique<detail::el>(std::move(tree_.get_dp()), tree_.get_data_ptr());
                    last_ = last_->add_next(std::make_unique<detail::bl>(last_->get_dp(),
                        last_->get_data_ptr(), el_ptr.get()));
                    el_ptr->set_bl_ptr(last_);
                    stack_el.push(std::move(el_ptr));
                    break;
                }
                case ']':
                {
                    if (stack_el.empty())
                        throw bad_brainfuck_string("Unmatched brackets appeared");
                    last_ = last_->add_next(std::move(stack_el.top()));
                    stack_el.pop();
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
        if (!stack_el.empty())
            throw bad_brainfuck_string("Unmatched brackets appeared");
    }

    void execute()
    {
        tree_.execute();
    }
private:
    detail::brainfuck_tree tree_;
    detail::op *last_;
    byte_t data_[size] = {0};
};


int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "You have to provide valid brainfuck string\n";
        exit(1);
    }
    brainfuck bf(argv[1]);
    bf.execute();
    return 0;
}
