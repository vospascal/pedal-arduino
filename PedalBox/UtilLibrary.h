#ifndef tl
#define tl
#include <EEPROM.h>
#include "Arduino.h"


class UtilLib  {
  public:
    // Constructor
    UtilLib();

    void arrayMapMultiplier(long *list, long multipier);

    void copyArray(long* src, long* dst, int len);

    String readStringFromEEPROM(int addrOffset);

    void writeStringToEEPROM(int addrOffset, const String &strToWrite);

    String generateStringMap(long *list);

    String generateStringMapCali(long *list);

    String getValue(String data, char separator, int index);

    long scaleMap(long pedalOutput, long lowDeadzone, long topDeadzone, long lowBits, long highBits);

    long scaleMultiMap(long value, long* inputMap, long* outputMap, int size);

  private:
};
#endif
