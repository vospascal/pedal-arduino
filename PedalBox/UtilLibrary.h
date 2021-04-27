#ifndef tl
#define tl
#include <EEPROM.h>

#if (ARDUINO >=100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class UtilLib  {
  public:
    // Constructor
    UtilLib();

    void arrayMapMultiplier(float *list, float multipier);

    void copyArray(int* src, float* dst, int len);

    String readStringFromEEPROM(int addrOffset);

    void writeStringToEEPROM(int addrOffset, const String &strToWrite);

    String generateStringMap(int *list);

    String generateStringMapCali(int *list);

    String getValue(String data, char separator, int index);

  private:
};
#endif
