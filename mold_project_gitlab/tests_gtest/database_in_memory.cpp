#include "database_in_memory.hpp"

database_in_memory::database_in_memory() : wolf::database{":memory:"} {}
