#include "udp_socket_in.h"
#include "util/logger.h"

#include <arpa/inet.h>
#include <unistd.h>

UDPSocketIn::UDPSocketIn(const sockaddr_in address) {
    /* Bind the address to the socket */
    if (bind(_socket_fd, (const sockaddr*)&address,
             sizeof(address)) == -1) {
        ERROR_LOG("Can't bind the address");
        close(_socket_fd);
        throw std::exception();
    }

    /* Print the listen address */
    TRACE_LOG("Listen on the %s:%hu",
              inet_ntoa(address.sin_addr),
              ntohs(address.sin_port));
}
