#include "buffer_pool.h"
#include "socket/udp_socket.h"

BufferPool::BufferPool(const int size) {
    /* Allocate the memory for all the buffers */
    for (int i = 0; i < size; ++i)
        _available_buffers.Push(new char[UDPSocket::MTU]);
}

char* BufferPool::Get() noexcept {
    /* Get the first available buffer */
    char* result = nullptr;
    try { result = _available_buffers.Head(); _available_buffers.Pop(); }
    catch (const std::exception&) { result = new char[UDPSocket::MTU]; }

    /* Mark the result as busy */
    _busy_buffers.Push(result);

    /* Return the result */
    return result;
}

void BufferPool::Release(char* buffer) noexcept {
    /* Mark the buffer as available */
    _busy_buffers.Remove(buffer);
    _available_buffers.Push(buffer);
}
