#include "calculation_state.hpp"

using namespace mold;

std::ostream &mold::operator<<(std::ostream &out, const calculation_state &to_print)
{
    switch (to_print) {
    case calculation_state::green:
        return out << "green";
    case calculation_state::yellow:
        return out << "yellow";
    case calculation_state::yellow_confirmed:
        return out << "yellow_confirmed";
    case calculation_state::red:
        return out << "red";
    case calculation_state::red_confirmed:
        return out << "red_confirmed";
    case calculation_state::undefined:
        return out << "undefined";
    }
    return out << "invalid";
}
enum struct calculation_state
{
    green,
    yellow,
    yellow_confirmed,
    red,
    red_confirmed,
    undefined
};
