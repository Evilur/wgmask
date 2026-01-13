#include "util/logger.h"
#include "socket/udp_socket.h"
#include "util/mutator.h"

#include <fstream>

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

static int print_help();

static int run(int argc, char* const* argv,
               void (*mutate1) (char* const, const short size),
               void (*mutate2) (char* const, const short size));

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
            return run(argc, argv, Mutator::DemaskPacket, Mutator::MaskPacket);
        }

        /* Find the client flag */
        if (strcmp(argv[i], "-c") == 0 ||
            strcmp(argv[i], "--client") == 0) {
            INFO_LOG("Run application as server");
            return run(argc, argv, Mutator::MaskPacket, Mutator::DemaskPacket);
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
               void (*mutate1) (char* const, const short),
               void (*mutate2) (char* const, const short)) {
    /* Get the addresses */
    UDPSocket* socket_in = nullptr;
    sockaddr_in remote_address { AF_INET }; remote_address.sin_port = 0;
    for (int i = 0; i < argc; i++)
        /* Get the local address */
        if (strcmp(argv[i], "-l") == 0 ||
            strcmp(argv[i], "--local") == 0) {
            socket_in = new UDPSocket();
            socket_in->Bind(UDPSocket::GetAddress(argv[i + 1]));
        }
        else if (strcmp(argv[i], "-r") == 0 ||
            strcmp(argv[i], "--remote") == 0) {
            remote_address = UDPSocket::GetAddress(argv[i + 1]);
        }

    /* Have we got all necessary addresses? */
    if (socket_in == nullptr || remote_address.sin_port == 0) {
        ERROR_LOG("Not all necessary addresses have been passed");
        return -1;
    }

    /* Wait for the UDP packages */
    for (;;) {
        /* Read the request from the client */
        char buffer[UDPSocket::MTU];
        sockaddr_in from { AF_INET };
        const long request_size = socket_in->Recieve(buffer, &from);

        TRACE_LOG("Get the request from %s:%hu",
                  inet_ntoa(from.sin_addr),
                  ntohs(from.sin_port));

        /* Mutate the package */
        mutate1(buffer, (short)request_size);

        /* Send the data to the server
         * and send the response to the client (async) */
        std::thread([&] {
            /* Send the data to the server */
            UDPSocket socket_out = UDPSocket();
            socket_out.Send(buffer, request_size, remote_address);
            TRACE_LOG("Proxy the request to the %s:%hu",
                      inet_ntoa(remote_address.sin_addr),
                      ntohs(remote_address.sin_port));

            /* Wait for all response packages */
            for (;;) {
                /* We have 5 seconds to get the response */
                timeval tval { };
                tval.tv_sec = 5;
                tval.tv_usec = 0;
                socket_out.SetOption(SO_RCVTIMEO, &tval, sizeof(timeval));

                /* Try to get the reponse */
                sockaddr_in server_from { AF_INET };
                const long response_size =
                    socket_out.Recieve(buffer, &server_from);
                if (response_size == -1) break;  //If there is no response
                TRACE_LOG("Recieve the responce from the %s:%hu",
                          inet_ntoa(server_from.sin_addr),
                          ntohs(server_from.sin_port));

                /* Mutate the package */
                mutate2(buffer, (short)request_size);

                /* If we get the response, send it to the client */
                socket_in->Send(buffer, response_size, from);
                TRACE_LOG("Proxy the response to the %s:%hu",
                          inet_ntoa(from.sin_addr),
                          ntohs(from.sin_port));
            }
        }).detach();
    }
    return 0;
}
