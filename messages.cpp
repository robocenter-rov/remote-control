#include "messages.h"

namespace Cmd {
    AxesValue::AxesValue(int16_t a0, int16_t a1, int16_t a2, int16_t a3, int16_t a4)
        : axis0(a0),
          axis1(a1),
          axis2(a2),
          axis3(a3),
          axis4(a4)
    {
    }
} //namespace Cmd
