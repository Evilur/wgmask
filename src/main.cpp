#include "container/dictionary.h"
#include "pool/buffer_pool.h"
#include "socket/udp_socket.h"
#include "util/logger.h"
#include "util/mutator.h"

#include <cstring>
#include <iostream>
#include <thread>
#include <unistd.h>

/* Dictionary to store the client_address:server_socket pairs */
static Dictionary<sockaddr_in, UDPSocket*> sockets(8);
static std::mutex sockets_mutex;

/* Thread pools */


/* Buffer pool */
static BufferPool buffer_pool(8);
static std::mutex buffers_mutex;

static int print_help();

static int client_loop(int argc, char* const* argv,
                       long (*mutate1) (char* const, const short),
                       long (*mutate2) (char* const, const short));

static void handle_client(const sockaddr_in& client_address,
                          const sockaddr_in& remote_address,
                          UDPSocket* client_socket,
                          char* request_buffer, long request_size,
                          long (*mutate1) (char* const, const short),
                          long (*mutate2) (char* const, const short));

static void server_loop(const sockaddr_in& client_address,
                       UDPSocket* client_socket,
                       UDPSocket* server_socket,
                       long (*mutate2) (char* const, const short));

static void handle_server(const sockaddr_in& client_address,
                          UDPSocket* client_socket,
                          char* response_buffer, long response_size,
                          long (*mutate2) (char* const, const short));

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
            INFO_LOG("Run application as server");
            return client_loop(argc, argv,
                            Mutator::DemaskPacket, Mutator::MaskPacket);
        }

        /* Find the client flag */
        if (strcmp(argv[i], "-c") == 0 ||
            strcmp(argv[i], "--client") == 0) {
            INFO_LOG("Run application as client");
            return client_loop(argc, argv,
                            Mutator::MaskPacket, Mutator::DemaskPacket);
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

static int client_loop(const int argc, char* const* const argv,
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

    /* Run the loop and wait for the UDP packages */
    for (;;) {
        /* Read the request from the client */
        buffers_mutex.lock();
        char* request_buffer = buffer_pool.Get();
        buffers_mutex.unlock();
        sockaddr_in client_address;
        long request_size = client_socket->Receive(request_buffer,
                                                   &client_address);

        /* If there is an error while receiving data */
        if (request_size == -1) {
            WARN_LOG("Error while receiving the request");
            continue;
        }

        /* Handle the client (async) */
        std::thread(handle_client, client_address, remote_address,
                    client_socket, request_buffer, request_size,
                    mutate1, mutate2).detach();
    }
    return 0;
}

static void handle_client(const sockaddr_in& client_address,
                          const sockaddr_in& remote_address,
                          UDPSocket* const client_socket,
                          char* const request_buffer, long request_size,
                          long (*const mutate1)
                          (char* const, const short size),
                          long (*const mutate2) (char* const, const short)) {
    /* Mutate the package */
    request_size = mutate1(request_buffer, (short)request_size);

    /* Try to get the server socket */
    UDPSocket* server_socket = nullptr;
    sockets_mutex.lock();
    try {
        server_socket = sockets.Get(client_address);
        sockets_mutex.unlock();
    } catch (const std::exception&) {
        /* If there is no a server socket yet for that client */
        server_socket = new UDPSocket();

        /* Bind the ephemeral address */
        server_socket->Bind(UDPSocket::EPHEMERAL_ADDRESS);

        /* Connect the socket to the server */
        server_socket->Connect(remote_address);

        /* Print the log */
        INFO_LOG("Save the %s:%hu client",
                 inet_ntoa(client_address.sin_addr),
                 ntohs(client_address.sin_port));

        /* If there are no data for 3 mins,
             * exit the thread and close the socket */
        timeval time {
            .tv_sec = (time_t)(60 * 3),
            .tv_usec = 0
        };
        server_socket->SetOption(SO_RCVTIMEO, &time, sizeof(time));

        /* Put that socket to the dictionary */
        sockets.Put(client_address, server_socket);
        sockets_mutex.unlock();

        /* Run the server loop to proxy the response to the client (async) */
        std::thread(server_loop, client_address, client_socket,
                    server_socket, mutate2).detach();
    }

    /* Send the data to the server */
    TRACE_LOG("Proxy the request to the server");
    server_socket->Send(request_buffer, request_size);
    buffers_mutex.lock();
    buffer_pool.Release(request_buffer);
    buffers_mutex.unlock();
}

static void server_loop(const sockaddr_in& client_address,
                       UDPSocket* const client_socket,
                       UDPSocket* const server_socket,
                       long (*const mutate2) (char* const, const short)) {
    for (;;) {
        /* Try to get a reponse */
        buffers_mutex.lock();
        char* response_buffer = buffer_pool.Get();
        buffers_mutex.unlock();
        long response_size = server_socket->Receive(response_buffer);

        /* If there is an error */
        if (response_size == -1) {
            /* Delete the socket from the dictionary */
            sockets_mutex.lock();
            delete server_socket;
            sockets.Delete(client_address);
            sockets_mutex.unlock();

            /* Print the log */
            INFO_LOG("Delete the %s:%hu client",
                     inet_ntoa(client_address.sin_addr),
                     ntohs(client_address.sin_port));

            /* Exit the loop (and thread) */
            break;
        }

        /* Handle the server (async) */
        std::thread(handle_server, client_address, client_socket,
                    response_buffer, response_size, mutate2).detach();
    }
}

static void handle_server(const sockaddr_in& client_address,
                          UDPSocket* const client_socket,
                          char* const response_buffer, long response_size,
                          long (*const mutate2) (char* const, const short)) {
    /* Mutate the package */
    response_size = mutate2(response_buffer,
                            (short)response_size);

    /* If we get the response, send it to the client */
    TRACE_LOG("Proxy to response from the server");
    client_socket->Send(response_buffer, response_size, client_address);
    buffers_mutex.lock();
    buffer_pool.Release(response_buffer);
    buffers_mutex.unlock();
}
