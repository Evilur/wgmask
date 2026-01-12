#pragma once

/**
 * A class to prevent objects instantiating
 * @author Evilur the.evilur@gmail.com
 */
class NonInstantiable {
public:
    NonInstantiable() = delete;
    ~NonInstantiable() = delete;

    NonInstantiable(const NonInstantiable&) = delete;
    NonInstantiable& operator=(const NonInstantiable&) = delete;

    NonInstantiable(NonInstantiable&&) = delete;
    NonInstantiable& operator=(NonInstantiable&&) = delete;
};
