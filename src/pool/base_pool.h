#pragma once

#include "util/class.h"

/**
 * A simple template for pool objects
 * @author Evilur the.evilur@gmail.com
 */
template <typename T>
class BasePool {
public:
    PREVENT_COPY_ALLOW_MOVE(BasePool)

    BasePool() = default;

    virtual ~BasePool() = default;

    virtual T* Get() noexcept = 0;

    virtual void Release(T* object) noexcept = 0;
};
