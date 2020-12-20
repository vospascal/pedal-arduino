// 11-10-2020
#include <Joystick.h>  // Using the lib included with SimHub originally from Matthew Heironimus
#include "MultiMap.h"
//#include <EEPROMex.h>

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD,
                   0, 0,                  // Button Count, Hat Switch Count
                   false, false, true,     // X and Y, Z Axis
                   false, false, false,   // Rx, Ry, or Rz
                   false, true,          // rudder or throttle
                   false, true, false);  // accelerator, brake, or steering


//const bool initAutoSendState = true;
int throttleValue = 0;
int brakeValue = 0;
int clutchValue = 0;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  Joystick.begin();
  Joystick.setThrottle(throttleValue);
  Joystick.setBrake(brakeValue);
  Joystick.setZAxis(clutchValue);
  delay(2000);
}


int inputMapClutch[6] =  { 0, 20, 40, 60, 80, 100 };
int outputMapClutch[6] = { 0, 15, 43, 53, 75, 100 };

int inputMapThrottle[6] =  { 0, 20, 40, 60, 80, 100 };
int outputMapThrottle[6] = { 0, 15, 43, 53, 75, 100 };

int inputMapBrake[6] =  { 0, 20, 40, 60, 80, 100 };
int outputMapBrake[6] = { 0, 15, 43, 53, 75, 100 };

int BrakeBefore;
float BrakeAfter;

int ThrottleBefore;
float ThrottleAfter;

int ClutchBefore;
float ClutchAfter;

// the loop routine runs over and over again forever:
void loop() {

  if (Serial.available() > 0) {
    String msg = Serial.readStringUntil('\n');

    if (msg.indexOf("Getmap") >= 0) {
      String TMAP = "TMAP:" + generateStringMap(outputMapThrottle);
      Serial.print(TMAP);
      Serial.println(',');

      String BMAP = "BMAP:" + generateStringMap(outputMapBrake);
      Serial.print(BMAP);
      Serial.println(',');

      String CMAP = "CMAP:" + generateStringMap(outputMapClutch);
      Serial.print(CMAP);
      Serial.println(',');
    }

    if (msg.indexOf("Setmap") >= 0) {
      //      Serial.print("setMap called");
      //      Serial.println(',');
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
    ThrottleAfter = multiMap<int>(ThrottleBefore, inputMapThrottle, outputMapThrottle, 50);
    Joystick.setThrottle(ThrottleAfter);
  }

  if (brakeRawValue <= 74) {
    BrakeBefore = 0;
    BrakeAfter = 0;
    Joystick.setBrake(0);
  } else {
    int restBrakeValue = brakeRawValue - 74;

    BrakeBefore = restBrakeValue / 4;
    BrakeAfter = multiMap<int>(BrakeBefore, inputMapBrake, outputMapBrake, 50);
    Joystick.setBrake(BrakeAfter);
  }

  if (clutchRawValue <= 74) {
    ClutchBefore = 0;
    ClutchAfter = 0;
    Joystick.setZAxis(0);
  } else {
    int restClutchValue = clutchRawValue - 74;

    ClutchBefore = restClutchValue / 4;
    ClutchAfter = multiMap<int>(ClutchBefore, inputMapClutch, outputMapClutch, 50);
    Joystick.setZAxis(ClutchAfter);
  }


  String p1 = ";";
  Serial.print("T:");
  Serial.println(ThrottleBefore + p1 + ThrottleAfter);

  Serial.print("B:");
  Serial.println(BrakeBefore + p1 + BrakeAfter);

  Serial.print("C:");
  Serial.println(ClutchBefore + p1 + ClutchAfter);

  Joystick.sendState(); // Update the Joystick status on the PC
  Serial.flush();
  delay(100);
}



//---------------------------------------------------------

//bool write_StringEEPROM(int Addr, String input) {
//  char charbuf[15];
//  input.toCharArray(charbuf, 15);
//
//  return EEPROM.writeBlock<char>(Addr, charbuf, 15);
//}
//
//bool update_StringEEPROM(int Addr, String input) {
//  char charbuf[15];
//  input.toCharArray(charbuf, 15);
//
//  return EEPROM.updateBlock<char>(Addr, charbuf, 15);
//}
//
//
//String read_StringEEPROM(int Addr) {
//  String outputEEPROM;
//  char output[] = " ";
//
//  EEPROM.readBlock<char>(Addr, output, 15);
//  //convert to string
//  outputEEPROM = String(output);
//
//  return outputEEPROM;
//}

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
  }
}


String getValue(String data, char separator, int index)
{
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
