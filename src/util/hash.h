#pragma once
#include <cstring>
#include <type_traits>

class Hash final {
public:
    template <typename T>
    static unsigned long Get(const T& element) noexcept;

private:
    static unsigned long Calculate(const unsigned char* element,
                                   unsigned short size) noexcept;
};

template <>
inline unsigned long Hash::Get(const char* const& element) noexcept {
    return Calculate((const unsigned char*)(void*)element, strlen(element) + 1);
}

template <>
inline unsigned long Hash::Get(char* const& element) noexcept {
    return Get((const char*)element);
}

template <typename T>
unsigned long Hash::Get(const T& element) noexcept {
    /* Get the byte array from the element and calc the hash */
    if constexpr (std::is_pointer_v<T>)
        return Get(*element);
    else
        return Calculate((const unsigned char*)(void*)&element,
                         sizeof(element));
}
