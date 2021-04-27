#include "UtilLibrary.h"

UtilLib::UtilLib() {
  // Anything you need when instantiating your object goes here
}

void UtilLib::arrayMapMultiplier(float *list, float multipier) {
  list[0] = (int)(list[0] * multipier);
  list[1] = (int)(list[1] * multipier);
  list[2] = (int)(list[2] * multipier);
  list[3] = (int)(list[3] * multipier);
  list[4] = (int)(list[4] * multipier);
  list[5] = (int)(list[5] * multipier);
}


void UtilLib::copyArray(int* src, float* dst, int len) {
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

String UtilLib::generateStringMap(int *list) {
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

String UtilLib::generateStringMapCali(int *list) {
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
