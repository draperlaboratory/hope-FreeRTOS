// Temporary fix - defines "uxrandom" function
#include "rand.h"
UBaseType_t uxRand(void) {
    return 42;
}
