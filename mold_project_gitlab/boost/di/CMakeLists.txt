find_path(boost_di_include_dir NAMES boost/di.hpp)
add_library(boost_di INTERFACE)
target_include_directories(boost_di INTERFACE ${boost_di_include_dir} ${boost_di_include_dir}/../extension/include)
add_library(wolf::boost_di ALIAS boost_di)
