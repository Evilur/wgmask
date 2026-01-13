#include "mutator.h"
#include "logger.h"

void Mutator::MaskPacket(char* const buffer, const short size) {
    TRACE_LOG("Mask the package");
    for (short i = 0; i < size; i++) buffer[i] ^= _mask[i];
}

void Mutator::DemaskPacket(char* const buffer, const short size) {
    TRACE_LOG("Demask the package");
    for (short i = 0; i < size; i++) buffer[i] ^= _mask[i];
}
