#ifndef __Timer__
#define __Timer__

#include "main.h"

#ifdef __cplusplus

extern "C" {
class Timer {
  public:
    // using DWT for timer watching
    Timer() {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }
    void reset() {
        startTime = DWT->CYCCNT;
    }

    uint32_t read_ms() {

        return (uint32_t)((float)(DWT->CYCCNT - startTime) / HAL_RCC_GetSysClockFreq() * 1000);
    }

    uint32_t read_us() {
        return (uint32_t)((float)(DWT->CYCCNT - startTime) / HAL_RCC_GetSysClockFreq() * 1000000);
    }

    uint32_t read_count() {
        return DWT->CYCCNT - startTime;
    }

  private:
    uint32_t startTime;
};
}

#endif
#endif