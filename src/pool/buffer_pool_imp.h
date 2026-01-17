#pragma once

#include "buffer_pool.h"
#include "socket/udp_socket.h"

template <int size>
BufferPool<size>::BufferPool() {
    /* Allocate the memory for all the buffers */
    for (int i = 0; i < size; i++)
        _available_buffers.Push(new char[UDPSocket::MTU]);
}

template <int size>
char* BufferPool<size>::Get() noexcept {
    /* Get the first available buffer */
    char* result = _available_buffers.Head();

    /* Mark the result as busy */
    if (result != nullptr) {
        _available_buffers.Pop();
        _busy_buffers.Push(result);
    }

    /* Return the result */
    return result;
}

template <int size>
void BufferPool<size>::Release(char* buffer) noexcept {
    /* Mark the buffer as available */
    _busy_buffers.Remove(buffer);
    _available_buffers.Push(buffer);
}
