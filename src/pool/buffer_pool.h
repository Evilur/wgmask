#pragma once

#include "base_pool.h"
#include "container/linked_list.h"

/**
 * @author Evilur the.evilur@gmail.com
 */
class BufferPool final : BasePool<char> {
public:
    explicit BufferPool(int size);

    char* Get() noexcept override;

    void Release(char* buffer) noexcept override;

private:
    LinkedList<char*> _available_buffers;
    LinkedList<char*> _busy_buffers;
};
