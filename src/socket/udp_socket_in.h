#pragma once
#include "udp_socket.h"

/**
 * Class to listen incoming UDP packages
 * @author Evilur the.evilur@gmail.com
 */
class UDPSocketIn final : public UDPSocket {
public:
    void Bind(sockaddr_in address) const;
};
