#pragma once

#include "util/class.h"

#ifdef _WIN64
#include <winsock2.h>
#include <ws2tcpip.h>
#undef ERROR
#undef INFO
#undef DEBUG
#undef WARN
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

/**
 * Basic class for UDP sockets
 * @author Evilur the.evilur@gmail.com
 */
class UDPSocket {
public:
    PREVENT_COPY_ALLOW_MOVE(UDPSocket)

    UDPSocket();

    ~UDPSocket() noexcept;

    void Bind(const sockaddr_in& address) const;

    void Connect(const sockaddr_in& address) const;

    long Receive(char* buffer, sockaddr_in* from) const noexcept;

    long Receive(char* buffer) const noexcept;

    void Send(const char* buffer, long buffer_size,
              const sockaddr_in& address) const noexcept;

    void Send(const char* buffer, long buffer_size) const noexcept;

    void SetOption(int optname, const void* optval,
                   socklen_t optlen) const noexcept;

    void Close() noexcept;

private:
    int _socket_fd = -1;

public:
    static constexpr int MTU = 1600;

    static inline const sockaddr_in EPHEMERAL_ADDRESS = {
        .sin_family = AF_INET,
        .sin_port = 0,
        .sin_addr = INADDR_ANY
    };

    static sockaddr_in GetAddress(char* str);
};
