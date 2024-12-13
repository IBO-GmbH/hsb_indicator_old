#ifndef CREATE_SHARED_WITH_INJECTOR_HPP
#define CREATE_SHARED_WITH_INJECTOR_HPP

#include <memory>

template <class create_type, class creator_type>
static std::shared_ptr<create_type> create_shared_with_injector(creator_type& injector) {
    return injector.template create<std::shared_ptr<create_type>>();
}

#endif
