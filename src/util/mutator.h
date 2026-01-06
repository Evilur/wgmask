#pragma once

#include "clear.h"

class Mutator final {
public:
    PREVENT_INSTANCE(Mutator);

    static void MaskPacket(char* buffer, short size);

    static void DemaskPacket(char* buffer, short size);
};
