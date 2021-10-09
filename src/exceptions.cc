#include "exceptions.hh"

namespace pd
{

bad_brainfuck_string::bad_brainfuck_string(const char *message)
    : message_(message) {}

const char*
bad_brainfuck_string::what() const noexcept
{ return message_; }

} // namespace pd
