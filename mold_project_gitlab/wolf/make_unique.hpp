#ifndef WOLF_MAKE_UNIQUE_HPP
#define WOLF_MAKE_UNIQUE_HPP

#include <memory>

namespace wolf
{

#if defined(__cpp_lib_make_unique)
#   error TODO, something with using!
    using std::make_uniqe;
#else
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
#endif

}

#endif // WOLF_MAKE_UNIQUE_HPP
