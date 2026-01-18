#pragma once

#include "base_pool.h"
#include "container/linked_list.h"

/**
 * @author Evilur the.evilur@gmail.com
 */
class BufferPool final : BasePool<char> {
public:
    PREVENT_COPY_ALLOW_MOVE(BufferPool);

    explicit BufferPool(int size);

    ~BufferPool() noexcept;

    char* Get() noexcept override;

    void Release(char* buffer) noexcept override;

private:
    LinkedList<char*> _available_buffers;
    LinkedList<char*> _busy_buffers;
};
