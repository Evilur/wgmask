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

    /* Dictionary to store the client_address:server_socket+timestamp pairs */
    struct Node {
      UDPSocket* server_socket = nullptr;
      std::time_t timestamp = 0;
    };
    Dictionary<sockaddr_in, Node> socket_pairs(8);

    /* Wait for the UDP packages */
    for (;;) {
        /* Read the request from the client */
        char* request_buffer = new char[UDPSocket::MTU];
        sockaddr_in client_address { AF_INET };
        long request_size = client_socket->Receive(request_buffer,
                                                   &client_address);

        /* If there is an error while receiving data */
        if (request_size == -1) {
            WARN_LOG("Error while receiving the request from %s:%hu",
                     inet_ntoa(client_address.sin_addr),
                     ntohs(client_address.sin_port));
            continue;
        }

        TRACE_LOG("Get the request from %s:%hu",
                  inet_ntoa(client_address.sin_addr),
                  ntohs(client_address.sin_port));

        /* Mutate the package */
        request_size = mutate1(request_buffer, (short)request_size);

        /* Send the data to the server
         * and send the response to the client (async) */
        std::thread([request_buffer, request_size,
                     &remote_address, client_address, client_socket,
                     &socket_pairs, mutate2] {
            /* Get the server socket or create a new one */
            UDPSocket* server_socket = nullptr;
            try {
                /* Try to get the socket pair from the dictionary */
                Node& socket_pair = socket_pairs.Get(client_address);

                /* Check the timestamp */
                std::time_t current_time = std::time(nullptr);
                if (current_time - socket_pair.timestamp > (long)(60 * 3)) {
                    delete socket_pair.server_socket;
                    socket_pair.server_socket = new UDPSocket();
                }

                /* Update the timestamp */
                socket_pair.timestamp = current_time;

                /* Get the server socket */
                server_socket = socket_pair.server_socket;
            } catch (const std::exception&) {
                /* If there is no such a pair in the dictionary,
                 * create a new one */
                server_socket = new UDPSocket();
                Node socket_pair {
                    .server_socket = server_socket,
                    .timestamp = std::time(nullptr)
                };

                /* Put the new pair to the dictionary */
                socket_pairs.Put(client_address, socket_pair);
            }

            /* Send the data to the server */
            server_socket->Send(request_buffer, request_size, remote_address);
            TRACE_LOG("Proxy the request to the %s:%hu",
                      inet_ntoa(remote_address.sin_addr),
                      ntohs(remote_address.sin_port));
            delete[] request_buffer;

            /* Wait for all response packages */
            for (;;) {
                /* We have 5 seconds to get the response */
                timeval tval { .tv_sec = 5, .tv_usec = 0 };
                server_socket->SetOption(SO_RCVTIMEO, &tval, sizeof(timeval));

                /* Try to get a reponse (a few seconds) */
                char response_buffer[UDPSocket::MTU];
                sockaddr_in server_address { AF_INET };
                long response_size =
                    server_socket->Receive(response_buffer, &server_address);

                /* If there is an error */
                if (response_size == -1) {
                    /* If the there is timeout error */
                    if (errno == EAGAIN || errno == EWOULDBLOCK) break;

                    /* If there is another error */
                    WARN_LOG("Error while receiving the request from %s:%hu",
                             inet_ntoa(server_address.sin_addr),
                             ntohs(server_address.sin_port));
                    continue;
                }

                /* If we've gotten the response */
                TRACE_LOG("Recieve the response from the %s:%hu",
                          inet_ntoa(server_address.sin_addr),
                          ntohs(server_address.sin_port));

                /* Mutate the package */
                response_size = mutate2(response_buffer, (short)response_size);

                /* If we get the response, send it to the client */
                client_socket->Send(response_buffer, response_size,
                                    client_address);
                TRACE_LOG("Proxy the response to the %s:%hu",
                          inet_ntoa(client_address.sin_addr),
                          ntohs(client_address.sin_port));
            }
        }).detach();
    }
    return 0;
}
