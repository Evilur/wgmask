#pragma once

#include <netinet/in.h>

/**
 * Basic class for UDP sockets
 * @author Evilur the.evilur@gmail.com
 */
class UDPSocket {
public:
    UDPSocket();

    ~UDPSocket() noexcept;

    UDPSocket(const UDPSocket& socket) noexcept = default;

    UDPSocket& operator=(const UDPSocket& socket) noexcept = default;

    UDPSocket(UDPSocket&& socket) noexcept = default;

    UDPSocket& operator=(UDPSocket&& socket) noexcept = default;

    void Bind(const sockaddr_in& address) const;

    long Recieve(char* buffer, sockaddr_in* from) const noexcept;

    void Send(const char* buffer, long buffer_size,
              const sockaddr_in& address) const noexcept;

    void SetOption(int optname, const void* optval,
                   socklen_t optlen) const noexcept;

private:
    const int _socket_fd = -1;

public:
    static constexpr int MTU = 1420;

    static sockaddr_in GetAddress(char* str);
};
