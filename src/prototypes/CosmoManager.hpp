#pragma once

extern "C" {
#include "cosmo.h"
#include "cstate.h"
}

namespace CosmoManager {
    extern CState *state;

    void init();
}