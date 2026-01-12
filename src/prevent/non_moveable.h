#pragma once

/**
 * A class to prevent objects move
 * @author Evilur the.evilur@gmail.com
 */
class NonMovable {
public:
    NonMovable(const NonMovable&) = delete;
    NonMovable& operator=(const NonMovable&) = delete;

    NonMovable(NonMovable&&) = delete;
    NonMovable& operator=(NonMovable&&) = delete;

protected:
    NonMovable() = default;
    ~NonMovable() = default;
};
