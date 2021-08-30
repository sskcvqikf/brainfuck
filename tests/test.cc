#include "../bf.hh"
#include <iostream>
#include <cmath>


void* print_testname(const char* name)
{
    std::cout << " ----- RUNNING " << name
        << " ----- \n";
    return nullptr;
}

bool is_failed = false;

#define TEST(name)                                      \
    void name(void* = print_testname(#name))            \

#define ASSERT(expr, message)                           \
    if (!(expr)){ is_failed = true;                     \
        std::cerr << "RUNNING: " << #expr               \
            << " - FAILED.\n" << __FILE__ << ":"        \
            << __LINE__ << " " << message << '\n';}     \

#define REQUIRE(expr)                                   \
    if (!(expr)){ is_failed = true;                     \
        std::cerr << "RUNNING: " << #expr               \
            << " - FAILED." << __FILE__ << ":"          \
            << __LINE__ << '\n';}                       \

#define ASSERT_THROW(expr, exc)                         \
    try {                                               \
        expr;                                           \
        is_failed = true;                               \
        std::cerr << "RUNNING: " << #expr               \
            << " - FAILED." << " No exception. "        \
            << __FILE__ << ":" << __LINE__ << '\n';     \
    } catch (exc&) {}                                   \
      catch (const std::exception& e) {                 \
            is_failed = true;                           \
            std::cerr << "RUNNING: " << #expr           \
                << " - FAILED." << " Other exception. " \
                << __FILE__ <<":"<< __LINE__  << '\n'   \
                << e.what() << '\n';}                   \
      catch (...) {                                     \
            is_failed = true;                           \
            std::cerr << "RUNNING: " << #expr           \
                << " - FAILED." << " Other thing. "     \
                << __FILE__ <<":"<< __LINE__  << '\n';} \

TEST(bad_brainfuck_string_test)
{
    REQUIRE((std::is_same_v<pd::byte_t, char>));
    ASSERT_THROW(pd::brainfuck{ ">>>>[" }, pd::bad_brainfuck_string);
    ASSERT_THROW(pd::brainfuck{ ">>>>]" }, pd::bad_brainfuck_string);
    ASSERT_THROW(pd::brainfuck{ "[[[]]]]" }, pd::bad_brainfuck_string);
    ASSERT_THROW(pd::brainfuck{ "[[[[]" }, pd::bad_brainfuck_string);

    for(pd::byte_t i{1}, max{std::numeric_limits<pd::byte_t>::max()};
            i < max ; ++i)
    {
        if ( i != '>' && i != '<' && i != '+'
             && i != '-' && i != '.' && i != ' '
             && i != '[' && i != ']' && i != '\t'
             && i != '\n' )
            ASSERT_THROW((pd::brainfuck(&i)), pd::bad_brainfuck_string);
    }
}

TEST(overflow_error_test)
{
    REQUIRE((std::is_same_v<pd::byte_t, char>));
    char overflow_right_1[30002] = {'>'};
    for(int i = 0; i < 30002; ++i)
    {
        overflow_right_1[i] = '>';
    }

    ASSERT_THROW((pd::brainfuck(overflow_right_1).execute()), std::overflow_error);
    char overflow_right_2[] = {"+[>+]"};
    ASSERT_THROW((pd::brainfuck(overflow_right_2).execute()), std::overflow_error);
    char overflow_left[] = {"<"};
    ASSERT_THROW((pd::brainfuck(overflow_left).execute()), std::overflow_error);
}

int main()
{
    bad_brainfuck_string_test();
    overflow_error_test();
    if (is_failed)
        exit(1);
    return 0;
}
