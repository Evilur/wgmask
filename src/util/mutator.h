#pragma once

#include "prevent/non_instantiable.h"

class Mutator final : NonInstantiable {
public:
    static void MaskPacket(char* buffer, short size);

    static void DemaskPacket(char* buffer, short size);
};
