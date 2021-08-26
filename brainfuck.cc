#include <iostream>
#include <memory>
#include <exception>
#include <algorithm>

using byte_t = char;
static constexpr int size = 30000;

namespace detail
{

struct op
{
    op(std::shared_ptr<int> dp, byte_t *data_ptr)
        : dp_(std::move(dp)), data_ptr_(data_ptr) {}

    void execute()
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
    
    std::shared_ptr<int> get_dp() { return dp_; }
    byte_t* get_data_ptr() { return data_ptr_; }
private:
    virtual void execute_impl() = 0;

    std::unique_ptr<op> nop_; // next instruction
    std::shared_ptr<int> dp_; // data ptr
    byte_t *data_ptr_;
};

struct idp : op // increment data pointer
{
    using op::op;

private:
    void execute_impl() override
    {
        if (++*get_dp() >= (size - 1))
            throw std::overflow_error("Data pointer is more than array size");
    }
};

struct ddp : op // decrement data pointer
{
    using op::op;

private:
    void execute_impl() override
    {
        if(--*get_dp() < 0)
            throw std::overflow_error("Data pointer is less than 0");
    }
};

struct id : op // increment byte in current cell
{
    using op::op;

private:
    void execute_impl() override
    {
        ++(*(get_data_ptr() + *get_dp()));
    }
};

struct dd : op // decrement byte in current cell
{
    using op::op;

private:
    void execute_impl() override
    {
        --(*(get_data_ptr() + *get_dp()));
    }
};

struct od : op // output byte in current cell
{
    using op::op;

private:
    void execute_impl() override
    {
        std::cout << (*(get_data_ptr() + *get_dp()));
    }
};

struct bl : op // begin loop
{
    using op::op;
};

struct el : op // end loop
{
    using op::op;
};

struct brainfuck_tree : op
{
    using op::op;
    void prepare() {}
private:
    void execute_impl(){}
};

} // namespace detail


struct brainfuck
{
    brainfuck(const char* bf)
        : tree_(std::make_shared<int>(0), data_), last_(&tree_)
    {
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
