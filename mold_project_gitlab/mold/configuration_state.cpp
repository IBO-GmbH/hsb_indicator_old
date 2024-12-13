#include "configuration_state.hpp"

#include <boost/optional/optional_io.hpp>


using namespace mold;


std::ostream &mold::operator<<(std::ostream &out, const configuration_state &to_print)
{
    const auto print_time_stamp = [&out](const configuration_state::time_point &print)
    {
        if (!print)
        {
            out << "--";
            return;
        }
        out << std::chrono::duration_cast<std::chrono::seconds>(print->time_since_epoch()).count();
    };
    out << "{state:" << to_print.state
        << " time_since_green:";
    print_time_stamp(to_print.time_since_green);
    out << " time_since_not_green:";
    print_time_stamp(to_print.time_since_not_green);
    out << " time_since_first_green:";
    print_time_stamp(to_print.time_since_first_green);
    return out << "}";
}

bool mold::configuration_state::operator==(const mold::configuration_state &compare) const
{
    return compare.state == state
            && compare.time_since_first_green == time_since_first_green
            && compare.time_since_green == time_since_green
            && compare.time_since_not_green == time_since_not_green;
}

bool configuration_state::operator!=(const configuration_state &compare) const
{
    return !(*this == compare);
}
