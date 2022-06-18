#include "UtilLibrary.h"

UtilLib::UtilLib() {
  // Anything you need when instantiating your object goes here
}

void UtilLib::arrayMapMultiplier(long *list, long multipier) {

  list[0] = long(float(list[0] * multipier));
  list[1] = long(float(list[1] * multipier));
  list[2] = long(float(list[2] * multipier));
  list[3] = long(float(list[3] * multipier));
  list[4] = long(float(list[4] * multipier));
  list[5] = long(float(list[5] * multipier));
}


void UtilLib::copyArray(long* src, long* dst, int len) {
  for (int i = 0; i < len; i++) {
    *dst++ = *src++;
  }
}

String UtilLib::readStringFromEEPROM(int addrOffset)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0'; // the character may appear in a weird way, you should read: 'only one backslash and 0'
  return String(data);
}

void UtilLib::writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  //  Serial.println(len);
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
}

String UtilLib::generateStringMap(long *list) {
  String output;
  for (int i = 0; i < 6; i++) {
    if (i < 5) {
      output += String(list[i]) + "-";
    }
    if (i == 5) {
      output += String(list[i]);
    }
    //    output += String(list[i]) += String(",");
  }
  return String(output);
}

String UtilLib::generateStringMapCali(long *list) {
  String output;
  for (int i = 0; i < 4; i++) {
    if (i < 3) {
      output += String(list[i]) + "-";
    }
    if (i == 3) {
      output += String(list[i]);
    }
    //    output += String(list[i]) += String(",");
  }
  return String(output);
}

String UtilLib::getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}


long UtilLib::scaleMap(long pedalOutput, long lowDeadzone, long topDeadzone, long lowBits, long highBits){
  return long( float(pedalOutput - lowDeadzone) * float(highBits - lowBits) / (topDeadzone - lowDeadzone) + lowBits);
}



long UtilLib::scaleMultiMap(long value, long* inputMap, long* outputMap, int size) {
  if (value <= inputMap[0]) return outputMap[0];
  if (value >= inputMap[size - 1]) return outputMap[size - 1];

  // search right interval
  uint8_t pos = 1;  // inputMap[0] allready tested
  while (value > inputMap[pos]) pos++;

  // this will handle all exact "points" in the _in array
  if (value == inputMap[pos]) return outputMap[pos];

  // interpolate in the right segment for the rest
  return long (
      float(
          float(value - inputMap[pos - 1]) *
          float(outputMap[pos] - outputMap[pos - 1])
      ) /
      float(inputMap[pos] - inputMap[pos - 1]) +
      outputMap[pos - 1]
  );
//  return (val - _in[pos - 1]) * (_out[pos] - _out[pos - 1]) / (_in[pos] - _in[pos - 1]) + _out[pos - 1];

}
