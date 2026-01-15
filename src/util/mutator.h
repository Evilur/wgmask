#pragma once

#include "util/class.h"

#include <cstring>

class Mutator final {
public:
    PREVENT_INSTANTIATION(Mutator)

    static long MaskPacket(char* buffer, short size);

    static long DemaskPacket(char* buffer, short size);

private:
    constexpr static const char* const MASK =
    "VLADIMIR PUTIN MOLODEC"
    "POLITIC LEADER I BOYEC";
    const static inline unsigned long MASK_SIZE = strlen(MASK);

    static constexpr int JUNK_BYTES = 8;

    enum PackageType : char {
        HANDSHAKE_INITATION = 1,
        HANDSHAKE_RESPONSE = 2,
        COOKIE_REPLY = 3,
        TRANSPORT_DATA = 4
    };
};
