#include <iostream>
#include "brainfuck.hh"

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "You have to provide valid brainfuck string\n";
        exit(1);
    }
    pd::brainfuck bf(argv[1]);
    bf.execute();
    return 0;
}
