//https://github.com/qub1750ul/Arduino_SoftwareReset
#pragma once

#include <avr/wdt.h>

namespace softwareReset
  {
    inline void simple()
      {
        asm volatile (" jmp 0");
      }

    inline void standard()
      {
        do
        {
            wdt_enable(WDTO_15MS);
            for(;;) {};

        } while(0);
      }

    // disable software reset after successful reset
    void disable() __attribute__((naked)) __attribute__((section(".init3"))) ;
    void disable()
      {
        MCUSR = 0 ;
        wdt_disable() ;
      }
  }
