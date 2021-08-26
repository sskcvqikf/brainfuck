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

    virtual void execute()
    {
        execute_impl();
        if (nop_.get() != nullptr)
            nop_.get()->execute();
    }

    virtual op* add_next(std::unique_ptr<op> nop)
    {
        nop_ = std::move(nop);
        return  nop_.get();
    }

    byte_t* get_data_ptr() { return data_ptr_; }
    op* get_nop_() { return nop_.get(); }
    std::shared_ptr<int> get_dp() { return dp_; }
    
private:
    virtual void execute_impl() = 0;

protected:
    std::unique_ptr<op> nop_; // next instruction
    std::shared_ptr<int> dp_; // data ptr
    byte_t *data_ptr_;
};

struct brainfuck_tree : op
{
    using op::op;
    void prepare() {}
private:
    void execute_impl(){}
};

struct idp : op // increment data pointer
{
    using op::op;

private:
    void execute_impl() override
    {
        if (++*dp_ >= (size - 1))
            throw std::overflow_error("Data pointer is more than array size");
    }
};

struct ddp : op // decrement data pointer
{
    using op::op;

private:
    void execute_impl() override
    {
        if(--*dp_ < 0)
            throw std::overflow_error("Data pointer is less than 0");
    }
};

struct id : op // increment byte in current cell
{
    using op::op;

private:
    void execute_impl() override
    {
        ++*(data_ptr_ + *dp_);
    }
};

struct dd : op // decrement byte in current cell
{
    using op::op;

private:
    void execute_impl() override
    {
        --*(data_ptr_ + *dp_);
    }
};

struct od : op // output byte in current cell
{
    using op::op;

private:
    void execute_impl() override
    {
        std::cout << *(data_ptr_ + *dp_);
    }
};

struct el : op // end loop
{
    using base = op;
    
    el(std::shared_ptr<int> dp, byte_t *data_ptr, op *bl_ptr)
        : base(dp, data_ptr), bl_ptr_(bl_ptr) {}

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

private:
    void execute_impl() override {}
    op* bl_ptr_ = nullptr;
};

struct bl : op // begin loop
{
    using op::op;

    void execute() override
    {
        if (*(data_ptr_ + *dp_) == 0)
        {
            if (nop_ != nullptr)
                after_el_->execute();
        }
        else
            nop_->execute();
    }

    op*& get_after_el()
    {
        return after_el_;
    }

private:
    void execute_impl() override {}
    op* after_el_;
};

} // namespace detail


struct brainfuck
{
    brainfuck(const char* bf)
        : tree_(std::make_shared<int>(0), data_), last_(&tree_)
    {
        std::stack<detail::bl*> stack_;
        for(const char* i = bf; *i != '\0'; ++i)
        {
            char di = *i;
            switch (di)
            {
                case '>':
                    last_ = last_->add_next(std::make_unique<detail::idp>(last_->get_dp(),
                        last_->get_data_ptr()));
                    break;
                case '<':
                    last_ = last_->add_next(std::make_unique<detail::ddp>(last_->get_dp(),
                        last_->get_data_ptr()));
                    break;
                case '+':
                    last_ = last_->add_next(std::make_unique<detail::id>(last_->get_dp(),
                        last_->get_data_ptr()));
                    break;
                case '-':
                    last_ = last_->add_next(std::make_unique<detail::dd>(last_->get_dp(),
                        last_->get_data_ptr()));
                    break;
                case '.':
                    last_ = last_->add_next(std::make_unique<detail::od>(last_->get_dp(),
                        last_->get_data_ptr()));
                    break;
                case '[':
                    last_ = last_->add_next(std::make_unique<detail::bl>(last_->get_dp(),
                        last_->get_data_ptr()));
                    stack_.push(dynamic_cast<detail::bl*>(last_));
                    break;
                case ']':
                    {
                    auto bl = stack_.top();
                    last_ = last_->add_next(std::make_unique<detail::el>(last_->get_dp(),
                        last_->get_data_ptr(), bl));
                    bl->get_after_el() = last_;
                    stack_.pop();
                    break;
                    }
                case '\n':
                    break;
                case '\t':
                    break;
                case ' ':
                    break;
                default:
                    throw std::invalid_argument("Unknown symbol appeared");
            }
        }
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
