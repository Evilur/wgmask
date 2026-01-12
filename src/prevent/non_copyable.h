#pragma once

/**
 * A class to prevent objects copy
 * @author Evilur the.evilur@gmail.com
 */
class NonCopyable {
public:
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;

    NonCopyable(NonCopyable&&) = default;
    NonCopyable& operator=(NonCopyable&&) = default;

protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
};
