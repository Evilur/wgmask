#include "hash.h"

unsigned long Hash::Calculate(const unsigned char* element,
                              const unsigned short size) noexcept {
    /* The variable to store the hash (751 - random prime number) */
    unsigned long hash = 751;

    /* Evaluate the hash */
    const unsigned char* const element_end = element + size;
    do {
        const unsigned char byte = *element;
        hash = (hash << 5) - hash + byte;
    } while (++element < element_end);

    /* Return the result */
    return hash;
}