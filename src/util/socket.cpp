#include "socket.h"
#include "logger.h"

#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

Socket::Socket() : _socket_fd(socket(AF_INET, SOCK_DGRAM, 0)) {
    /* Check the socket */
    if (_socket_fd == -1) {
        ERROR_LOG("Can't create a socket");
        throw std::exception();
    }
}

void Socket::Bind(char* const str) const {
    /* Store the local address */
    sockaddr_in local_address { AF_INET };

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
    if (inet_aton(ip_ptr, &local_address.sin_addr) == 0) {
        ERROR_LOG("Invalid ip address");
        throw std::exception();
    }

    /* Set the port */
    local_address.sin_port = htons((unsigned short)atoi(port_ptr));
    if (local_address.sin_port == 0) {
        ERROR_LOG("Invalid port");
        throw std::exception();
    }

    /* Bind the address to the socket */
    if (bind(_socket_fd, (sockaddr*)&local_address,
             sizeof(local_address)) == -1) {
        ERROR_LOG("Can't bind the address");
        close(_socket_fd);
        throw std::exception();
    }

    /* Print the listen address */
    TRACE_LOG("Listen on the %s:%hu",
              inet_ntoa(local_address.sin_addr),
              ntohs(local_address.sin_port));
}

int Socket::Recieve(char* const buffer) const {

}
