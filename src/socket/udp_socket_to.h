#pragma once

#include "socket/udp_socket.h"

/**
 * Class to send UDP packages to the remove host
 * @author Evilur the.evilur@gmail.com
 */
class UDPSocketTo final : public UDPSocket {
public:
    explicit UDPSocketTo(sockaddr_in address) noexcept;

private:
    sockaddr_in _address;
};
