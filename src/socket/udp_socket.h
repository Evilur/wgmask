#pragma once

#include <netinet/in.h>

/**
 * Basic class for UDP sockets
 * @author Evilur the.evilur@gmail.com
 */
class UDPSocket {
protected:
    int _socket_fd = -1;

    UDPSocket();

    ~UDPSocket() noexcept;

    UDPSocket(const UDPSocket& socket) noexcept = default;

    UDPSocket& operator=(const UDPSocket& socket) noexcept = default;

    UDPSocket(UDPSocket&& socket) noexcept = default;

    UDPSocket& operator=(UDPSocket&& socket) noexcept = default;

public:
    static constexpr int MTU = 1420;

    static sockaddr_in GetAddress(char* str);
};
