#ifndef WOLF_STRONG_TYPE_HPP
#define WOLF_STRONG_TYPE_HPP

#include <memory>

namespace wolf
{

template <typename Type>
class strong_type
{
public:
    explicit strong_type(Type const& value) : value_(value) {}
    explicit strong_type(Type&& value) : value_(std::move(value)) {}
    Type& get() { return value_; }
    Type const& get() const {return value_; }
private:
    Type value_;
};

}

#endif // WOLF_STRONG_TYPE_HPP
