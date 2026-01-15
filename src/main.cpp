#include "container/dictionary.h"
#include "util/logger.h"
#include "socket/udp_socket.h"
#include "util/mutator.h"

#include <cstddef>
#include <fstream>
#include <format>

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

static int print_help();

static int run(int argc, char* const* argv,
               long (*mutate1) (char* const, const short size),
               long (*mutate2) (char* const, const short size));

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
               long (*mutate1) (char* const, const short),
               long (*mutate2) (char* const, const short)) {
    /* Get the addresses */
    UDPSocket* client_socket = nullptr;
    sockaddr_in remote_address { AF_INET }; remote_address.sin_port = 0;
    for (int i = 0; i < argc; i++)
        /* Get the local address */
        if (strcmp(argv[i], "-l") == 0 ||
            strcmp(argv[i], "--local") == 0) {
            client_socket = new UDPSocket();
            client_socket->Bind(UDPSocket::GetAddress(argv[i + 1]));
        }
        else if (strcmp(argv[i], "-r") == 0 ||
            strcmp(argv[i], "--remote") == 0) {
            remote_address = UDPSocket::GetAddress(argv[i + 1]);
        }

    /* Have we got all necessary addresses? */
    if (client_socket == nullptr || remote_address.sin_port == 0) {
        ERROR_LOG("Not all necessary addresses have been passed");
        return -1;
    }

    /* Wait for the UDP packages */
    for (;;) {
        /* Read the request from the client */
        char request_buffer[UDPSocket::MTU];
        sockaddr_in client_address { AF_INET };
        long request_size = client_socket->Receive(request_buffer,
                                                   &client_address);

        /* If there is an error while receiving data */
        if (request_size == -1) {
            WARN_LOG("Error while receiving the request");
            continue;
        }

        /* Mutate the package */
        request_size = mutate1(request_buffer, (short)request_size);

        /* Dictionary to store the client_address:server_socket pairs */
        static Dictionary<sockaddr_in, UDPSocket*> sockets(8);

        /* Try to get the server socket */
        UDPSocket* server_socket = nullptr;
        bool server_thread_exists = false;
        try {
            server_socket = sockets.Get(client_address);
            server_thread_exists = true;
        } catch (const std::exception&) {
            server_socket = new UDPSocket();
            server_socket->Bind(UDPSocket::EPHEMERAL_ADDRESS);
            server_socket->Connect(remote_address);
            sockets.Put(client_address, server_socket);
        }

        /* Send the data to the server */
        TRACE_LOG("Proxy the request to the server");
        server_socket->Send(request_buffer, request_size);

        /* If we already has the server thread, continue the loop */
        if (server_thread_exists) continue;

        /* Send the data to the server
         * and send the response to the client (async) */
        std::thread([client_socket, server_socket, client_address, mutate2] {
            for (;;) {
                /* Try to get a reponse */
                char response_buffer[UDPSocket::MTU];
                long response_size =
                    server_socket->Receive(response_buffer);

                /* If there is an error */
                if (response_size == -1) {
                    WARN_LOG("Error while receiving the response");
                    continue;
                }

                /* Mutate the package */
                response_size = mutate2(response_buffer, (short)response_size);

                /* If we get the response, send it to the client */
                TRACE_LOG("Proxy to response from the server");
                client_socket->Send(response_buffer, response_size,
                                    client_address);
            }
        }).detach();
    }
    return 0;
}
