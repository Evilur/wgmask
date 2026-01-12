#include "util/logger.h"
#include "socket/udp_socket.h"
#include "socket/udp_socket_in.h"
#include "socket/udp_socket_to.h"
#include "util/mutator.h"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

static int print_help();

static int run(int argc, char* const* argv,
               void (*mutant_package) (char* const, const short size));

int main(int argc, char** argv) {
    /* Read basic flags */
    for (int i = 1; i < argc; i++) {
        /* Find the help flag */
        if (strcmp(argv[i], "-h") == 0 ||
            strcmp(argv[i], "--help") == 0)
            return print_help();

        /* Find the server flag */
        if (strcmp(argv[i], "-s") == 0 ||
            strcmp(argv[i], "--server") == 0) {
            INFO_LOG("Run application as client");
            return run(argc, argv, Mutator::MaskPacket);
        }

        /* Find the client flag */
        if (strcmp(argv[i], "-c") == 0 ||
            strcmp(argv[i], "--client") == 0) {
            INFO_LOG("Run application as server");
            return run(argc, argv, Mutator::DemaskPacket);
        }
    }

    /* If there is no basic flags, print the help message and exit */
    return print_help();
}

static int print_help() {
    std::cout
        << "NAME:\n"
        << "\twgmask\n"
        << "DESCRIPTION\n"
        << "\tMask the wireguard traffic to pass the DPI\n"
        << "USAGE\n"
        << "\twgmask [arguments]\n"
        << "ARGUMENTS\n"
        << "\t-h, --help\n"
        << "\t\tPrint the help and exit\n"
        << "\t-c, --client\n"
        << "\t\tRun the application in the client mode\n"
        << "\t-s, --server\n"
        << "\t\tRun the application in the server mode\n"
        << "\t-l, --local\n"
        << "\t\tSpecify the local address\n"
        << "\t-r, --remote\n"
        << "\t\tSpecify the remote address\n"
        << std::flush;
    return 0;
}

static int run(const int argc, char* const* const argv,
               void (*mutant_package) (char* const, const short)) {
    /* Get the addresses */
    UDPSocketIn* socket_in = nullptr;
    UDPSocketTo* socket_to = nullptr;
    for (int i = 0; i < argc; i++)
        /* Get the local address */
        if (strcmp(argv[i], "-l") == 0 ||
            strcmp(argv[i], "--local") == 0) {
            socket_in = new UDPSocketIn(UDPSocket::GetAddress(argv[i + 1]));
        }
        else if (strcmp(argv[i], "-r") == 0 ||
            strcmp(argv[i], "--remote") == 0) {
            socket_to = new UDPSocketTo(UDPSocket::GetAddress(argv[i + 1]));
        }

    /* Have we got all necessary addresses? */
    if (socket_in == nullptr || socket_to == nullptr) {
        ERROR_LOG("Not all necessary addresses have been passed");
        return -1;
    }

    /* Wait for the UDP packages */
    char buffer[UDPSocketIn::MTU];
    for (;;) {

    }
    return 0;
}
