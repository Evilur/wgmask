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
