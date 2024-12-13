#ifndef STRING_PRINT_HELPER_HPP
#define STRING_PRINT_HELPER_HPP

#include <string>

class string_print_helper
{
public:
    static std::string as_hex(const std::string &to_print);
    static std::string from_at_string(const std::string &to_print);
};

#endif // STRING_PRINT_HELPER_HPP
