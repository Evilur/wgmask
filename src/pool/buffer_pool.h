#pragma once

#include "base_pool.h"
#include "container/linked_list.h"

/**
 * @author Evilur the.evilur@gmail.com
 */
template <int size = 32>
class BufferPool final : BasePool<char> {
public:
    BufferPool();

    char* Get() noexcept override;

    void Release(char* buffer) noexcept override;

private:
    LinkedList<char*> _available_buffers;
    LinkedList<char*> _busy_buffers;
};

#include "buffer_pool_imp.h"
