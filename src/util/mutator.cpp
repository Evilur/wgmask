#include "mutator.h"
#include "logger.h"

long Mutator::MaskPacket(char* const buffer, const short size) {
    TRACE_LOG("Mask the package");

    /* Get the package type
     * (Wireguard uses them to distinguish package types) */
    PackageType type = (PackageType)buffer[0];

    /* Mask first bytes */
    #pragma unroll
    for (short i = 0; i < 4; ++i) buffer[i] ^= MASK[i];

    /* If there is a handshake request */
    if (type == HANDSHAKE_INITATION)
        #pragma unroll
        for (short i = 148 - 16; i < 148; ++i)
            buffer[i] ^= MASK[i % MASK_SIZE];
    /* If there is a handshake response */
    else if (type == HANDSHAKE_RESPONSE)
        #pragma unroll
        for (short i = 92 - 16; i < 92; ++i) buffer[i] ^= MASK[i % MASK_SIZE];

    /* Return the current buffer size with junk bytes from the memory */
    return size + (type < COOKIE_REPLY ? JUNK_BYTES : 0);
}

long Mutator::DemaskPacket(char* const buffer, const short size) {
    TRACE_LOG("Demask the package");

    /* Demask first bytes
     * (Wireguard uses them to distinguish package types) */
    #pragma unroll
    for (short i = 0; i < 4; ++i) buffer[i] ^= MASK[i];

    /* Get the package type */
    PackageType type = (PackageType)buffer[0];

    /* If there is a handshake request */
    if (type == HANDSHAKE_INITATION)
        #pragma unroll
        for (short i = 148 - 16; i < 148; ++i)
            buffer[i] ^= MASK[i % MASK_SIZE];
    /* If there is a handshake response */
    else if (type == HANDSHAKE_RESPONSE)
        #pragma unroll
        for (short i = 92 - 16; i < 92; ++i) buffer[i] ^= MASK[i % MASK_SIZE];

    /* Return the current buffer size with junk bytes from the memory */
    return size + (type < COOKIE_REPLY ? JUNK_BYTES : 0);
}
