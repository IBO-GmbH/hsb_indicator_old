#include "bond_sender.hpp"

wolf::network::bond_sender::bond_sender(wolf::websocket_sync &destination)
    : m_destination(destination) {}
