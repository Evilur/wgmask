#include "udp_socket.h"
#include "util/logger.h"

#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

UDPSocket::UDPSocket() : _socket_fd(socket(AF_INET, SOCK_DGRAM, 0)) {
    /* Check the socket */
    if (_socket_fd == -1) {
        ERROR_LOG("Can't create a socket");
        throw std::exception();
    }
}

UDPSocket::~UDPSocket() noexcept { if (_socket_fd != -1 ) close(_socket_fd); }

void UDPSocket::Bind(const sockaddr_in& address) const {
    /* Bind the address to the socket */
    if (bind(_socket_fd, (const sockaddr*)&address, sizeof(address)) == -1) {
        ERROR_LOG("Can't bind the address");
        close(_socket_fd);
        throw std::exception();
    }

    /* Set options */
    const int reuse = 1;
    SetOption(SO_REUSEADDR, &reuse, sizeof(reuse));
    SetOption(SO_REUSEPORT, &reuse, sizeof(reuse));

    /* Print the listen address */
    INFO_LOG("Listen on the %s:%hu",
             inet_ntoa(address.sin_addr),
             ntohs(address.sin_port));
}

void UDPSocket::Connect(const sockaddr_in& address) const {
    /* Connect to the address */
    if (connect(_socket_fd, (const sockaddr*)&address, sizeof(address)) ==
        -1) {
        ERROR_LOG("Can't connect ot the server");
        close(_socket_fd);
        throw std::exception();
    }

    /* Print the connect address */
    INFO_LOG("Connect to %s:%hu",
             inet_ntoa(address.sin_addr),
             ntohs(address.sin_port));
}

long UDPSocket::Receive(char* buffer, sockaddr_in* from)
const noexcept {
    /* Send the data */
    socklen_t from_len = sizeof(sockaddr_in);
    const long result = recvfrom(_socket_fd, buffer, MTU, 0,
                           (sockaddr*)from, &from_len);

    /* If there is an error */
    if (result == -1) {
        WARN_LOG("Error while receiving the data");
        return -1;
    }

    /* Print the log */
    TRACE_LOG("Get %lu bytes from %s:%hu",
              result,
              inet_ntoa(from->sin_addr),
              ntohs(from->sin_port));

    /* Return the result */
    return result;
}

long UDPSocket::Receive(char* buffer)
const noexcept {
    const long result = recv(_socket_fd, buffer, MTU, 0);

    /* If there is an error */
    if (result == -1) {
        WARN_LOG("Error while receiving the data");
        return -1;
    }

    /* Print the log */
    #if LOG_LEVEL == 0
    sockaddr_in from;
    socklen_t from_len = sizeof(from);
    getpeername(_socket_fd, (sockaddr*)&from, &from_len);
    TRACE_LOG("Get %lu bytes from %s:%hu",
              result,
              inet_ntoa(from.sin_addr),
              ntohs(from.sin_port));
    #endif

    /* Return the result */
    return result;
}

void UDPSocket::Send(const char* const buffer, const long buffer_size,
                     const sockaddr_in& address) const noexcept {
    /* Send the data */
    const long result  = sendto(_socket_fd, buffer,
                                (unsigned long)buffer_size, 0,
                                (const sockaddr*)&address,
                                sizeof(sockaddr_in));

    /* If there is an error */
    if (result == -1) {
        WARN_LOG("Error while sending the data");
        return;
    }

    /* Print the log */
    TRACE_LOG("Send %lu bytes to %s:%hu",
              result,
              inet_ntoa(address.sin_addr),
              ntohs(address.sin_port));
}

void UDPSocket::Send(const char* const buffer, const long buffer_size)
const noexcept {
    /* Send the data */
    const long result  = send(_socket_fd, buffer,
                              (unsigned long)buffer_size, 0);

    /* If there is an error */
    if (result == -1) {
        WARN_LOG("Error while sending the data");
        return;
    }

    /* Print the log */
    #if LOG_LEVEL == 0
    sockaddr_in address;
    socklen_t address_len = sizeof(address);
    getpeername(_socket_fd, (sockaddr*)&address, &address_len);
    TRACE_LOG("Send %lu bytes to %s:%hu",
              result,
              inet_ntoa(address.sin_addr),
              ntohs(address.sin_port));
    #endif
}

void UDPSocket::SetOption(const int optname, const void* const optval,
                          const socklen_t optlen) const noexcept {
    if (setsockopt(_socket_fd, SOL_SOCKET, optname, optval, optlen) == -1)
        WARN_LOG("Can't set socket option");
}

void UDPSocket::Close() noexcept {
    close(_socket_fd);
    _socket_fd = -1;
}

sockaddr_in UDPSocket::GetAddress(char* const str) {
    /* Store the result */
    sockaddr_in result { AF_INET };

    /* Get the ip */
    char* const ip_ptr = str;
    char* const ip_end = strchr(ip_ptr, ':');
    if (ip_end == nullptr) {
        ERROR_LOG("Invalid address");
        throw std::exception();
    }
    *ip_end = '\0';

    /* Get the port */
    const char* const port_ptr = ip_end + 1;

    /* Set the ip */
    if (inet_aton(ip_ptr, &result.sin_addr) == 0) {
        ERROR_LOG("Invalid ip address");
        throw std::exception();
    }

    /* Set the port */
    result.sin_port = htons((unsigned short)atoi(port_ptr));
    if (result.sin_port == 0) {
        ERROR_LOG("Invalid port");
        throw std::exception();
    }

    /* Return the result */
    return result;
}
