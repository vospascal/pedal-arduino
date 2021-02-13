// 11-10-2020
#include <Joystick.h>  // Using the lib included with SimHub originally from Matthew Heironimus
#include "MultiMap.h"
#include <EEPROM.h>

#define E_INIT 1023
#define E_CLUTCH 0
#define E_THROTTLE 30
#define E_BRAKE 60

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD,
                   0, 0,                  // Button Count, Hat Switch Count
                   false, false, true,     // X and Y, Z Axis
                   false, false, false,   // Rx, Ry, or Rz
                   false, true,          // rudder or throttle
                   false, true, false);  // accelerator, brake, or steering

int clutchValue = 0;
int inputMapClutch[6] =  { 0, 20, 40, 60, 80, 100 };
int outputMapClutch[6] = { 0, 20, 40, 60, 80, 100 };
int ClutchBefore;
int ClutchAfter;
int ClutchRaw;

int throttleValue = 0;
int inputMapThrottle[6] =  { 0, 20, 40, 60, 80, 100 };
int outputMapThrottle[6] = { 0, 20, 40, 60, 80, 100 };
int ThrottleBefore;
int ThrottleAfter;
int ThrottleRaw;

int brakeValue = 0;
int inputMapBrake[6] =  { 0, 20, 40, 60, 80, 100 };
int outputMapBrake[6] = { 0, 20, 40, 60, 80, 100 };
int BrakeBefore;
int BrakeAfter;
int BrakeRaw;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  loadEEPROMSettings();
  Joystick.begin();
  Joystick.setThrottle(throttleValue);
  Joystick.setBrake(brakeValue);
  Joystick.setZAxis(clutchValue);
  delay(2000);
}

// the loop routine runs over and over again forever:
void loop() {
  if (Serial.available() > 0) {
    String msg = Serial.readStringUntil('\n');
    String cm = ",";

    if (msg.indexOf("Getmap") >= 0) {
      String TMAP = "TMAP:" + generateStringMap(outputMapThrottle);
      String BMAP = "BMAP:" + generateStringMap(outputMapBrake);
      String CMAP = "CMAP:" + generateStringMap(outputMapClutch);
      Serial.println(TMAP + cm + BMAP + cm + CMAP);
    }

    updateThrottleMap(msg);
    updateBrakeMap(msg);
    updateClutchMap(msg);
  }


  // read the input on analog pins
  int throttleRawValue = analogRead(A0);
  int brakeRawValue = analogRead(A3);
  int brakePresureRawValue = analogRead(A2);
  int clutchRawValue = analogRead(A1);

  // print out the value you read:
  if (throttleRawValue <= 74) {
    ThrottleBefore = 0;
    ThrottleAfter = 0;
    Joystick.setThrottle(0);
  } else {
    int restThrottleValue = throttleRawValue - 74;
    ThrottleBefore = restThrottleValue / 4;
    ThrottleAfter = multiMap<int>(ThrottleBefore, inputMapThrottle, outputMapThrottle, 6);
    ThrottleRaw = throttleRawValue;
    Joystick.setThrottle(ThrottleAfter);
  }

  if (brakeRawValue <= 74) {
    BrakeBefore = 0;
    BrakeAfter = 0;
    Joystick.setBrake(0);
  } else {
    int restBrakeValue = brakeRawValue - 74;
    BrakeBefore = restBrakeValue / 4;
    BrakeAfter = multiMap<int>(BrakeBefore, inputMapBrake, outputMapBrake, 6);
    BrakeRaw = brakeRawValue;
    Joystick.setBrake(BrakeAfter);
  }

  if (clutchRawValue <= 74) {
    ClutchBefore = 0;
    ClutchAfter = 0;
    Joystick.setZAxis(0);
  } else {
    int restClutchValue = clutchRawValue - 74;
    ClutchBefore = restClutchValue / 4;
    ClutchAfter = multiMap<int>(ClutchBefore, inputMapClutch, outputMapClutch, 6);
    ClutchRaw = clutchRawValue;
    Joystick.setZAxis(ClutchAfter);
  }


  String p1 = ";";
  String cm = ",";

  String throttleStringPrefix = "T:";
  String throttleStringValues = ThrottleBefore + p1 + ThrottleAfter + p1 + ThrottleRaw +cm;
  String throttleString = throttleStringPrefix + throttleStringValues;

  String brakeStringPrefix = "B:";
  String brakeStringValues = BrakeBefore + p1 + BrakeAfter + p1 + BrakeRaw + cm;
  String brakeString = brakeStringPrefix + brakeStringValues;

  String clutchStringPrefix = "C:";
  String clutchStringValues = ClutchBefore + p1 + ClutchAfter + p1 + ClutchRaw + cm;
  String clutchString = clutchStringPrefix + clutchStringValues;

  Serial.println(throttleString + brakeString + clutchString);
  Joystick.sendState(); // Update the Joystick status on the PC
  //  Serial.flush();
  //  delay(150);
}


//---------------------------------------------------------

void loadEEPROMSettings() {
  if (EEPROM.read(E_INIT) == 'T') {
     //read
    String EEPROM_ClutchMap = readStringFromEEPROM(0);
    String CMAP = "CMAP:";
    updateClutchMap(CMAP + EEPROM_ClutchMap);

    String EEPROM_ThrottleMap = readStringFromEEPROM(30);
    String TMAP = "TMAP:";
    updateThrottleMap(TMAP + EEPROM_ThrottleMap);

    String EEPROM_BrakeMap = readStringFromEEPROM(60);
    String BMAP = "BMAP:";
    updateBrakeMap(BMAP + EEPROM_BrakeMap);
  } else {
    // write
    EEPROM.write(E_INIT, 'T');
    writeStringToEEPROM(0, generateStringMap(outputMapClutch));
    writeStringToEEPROM(30, generateStringMap(outputMapThrottle));
    writeStringToEEPROM(60, generateStringMap(outputMapBrake));
  }

}

void writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  //  Serial.println(len);
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
}

String readStringFromEEPROM(int addrOffset)
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


//---------------------------------------------------------

String generateStringMap(int *lists) {
  String output;
  for (int i = 0; i < 6; i++) {
    if (i < 5) {
      output += String(lists[i]) + "-";
    }
    if (i == 5) {
      output += String(lists[i]);
    }
    //    output += String(lists[i]) += String(",");
  }
  return String(output);

}


void updateThrottleMap (String msg) {
  if (msg.indexOf("TMAP:") >= 0) {
    String striped_Tmap = msg;
    striped_Tmap.replace("TMAP:", "");
    String tpart0 = getValue(striped_Tmap, '-', 0);
    String tpart1 = getValue(striped_Tmap, '-', 1);
    String tpart2 = getValue(striped_Tmap, '-', 2);
    String tpart3 = getValue(striped_Tmap, '-', 3);
    String tpart4 = getValue(striped_Tmap, '-', 4);
    String tpart5 = getValue(striped_Tmap, '-', 5);
    outputMapThrottle[0] = tpart0.toInt();
    outputMapThrottle[1] = tpart1.toInt();
    outputMapThrottle[2] = tpart2.toInt();
    outputMapThrottle[3] = tpart3.toInt();
    outputMapThrottle[4] = tpart4.toInt();
    outputMapThrottle[5] = tpart5.toInt();

    // update EEPROM settings
    writeStringToEEPROM(30, generateStringMap(outputMapThrottle));
  }
}


void updateBrakeMap(String msg) {
  if (msg.indexOf("BMAP:") >= 0) {
    String striped_Bmap = msg;
    striped_Bmap.replace("BMAP:", "");
    String bpart0 = getValue(striped_Bmap, '-', 0);
    String bpart1 = getValue(striped_Bmap, '-', 1);
    String bpart2 = getValue(striped_Bmap, '-', 2);
    String bpart3 = getValue(striped_Bmap, '-', 3);
    String bpart4 = getValue(striped_Bmap, '-', 4);
    String bpart5 = getValue(striped_Bmap, '-', 5);
    outputMapBrake[0] = bpart0.toInt();
    outputMapBrake[1] = bpart1.toInt();
    outputMapBrake[2] = bpart2.toInt();
    outputMapBrake[3] = bpart3.toInt();
    outputMapBrake[4] = bpart4.toInt();
    outputMapBrake[5] = bpart5.toInt();

    // update EEPROM settings
    writeStringToEEPROM(60, generateStringMap(outputMapBrake));
  }
}


void updateClutchMap(String msg) {
  if (msg.indexOf("CMAP:") >= 0) {
    String striped_Cmap = msg;
    striped_Cmap.replace("CMAP:", "");
    String cpart0 = getValue(striped_Cmap, '-', 0);
    String cpart1 = getValue(striped_Cmap, '-', 1);
    String cpart2 = getValue(striped_Cmap, '-', 2);
    String cpart3 = getValue(striped_Cmap, '-', 3);
    String cpart4 = getValue(striped_Cmap, '-', 4);
    String cpart5 = getValue(striped_Cmap, '-', 5);
    outputMapClutch[0] = cpart0.toInt();
    outputMapClutch[1] = cpart1.toInt();
    outputMapClutch[2] = cpart2.toInt();
    outputMapClutch[3] = cpart3.toInt();
    outputMapClutch[4] = cpart4.toInt();
    outputMapClutch[5] = cpart5.toInt();

    // update EEPROM settings
    writeStringToEEPROM(0, generateStringMap(outputMapClutch));
  }
}


String getValue(String data, char separator, int index) {
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
