// if arduino doest upload wire RST to GND to reset and upload empty sketch

#include <Joystick.h>
#include "MultiMap.h"
#include "SoftwareReset.h"
#include <EEPROM.h>

#include "UtilLibrary.h"

#include "Pedal.h"

#define E_INIT 1023
#define E_CLUTCH 0
#define E_THROTTLE 30
#define E_BRAKE 60
#define E_CALIBRATION_C 90
#define E_CALIBRATION_B 120
#define E_CALIBRATION_T 150
#define E_PEDAL_INVERTED_MAP 200
#define SENSOR_RANGE 1023
#define SERIAL_RANGE 100

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD,
                   0, 0,                  // Button Count, Hat Switch Count
                   false, false, true,     // X and Y, Z Axis
                   false, false, false,   // Rx, Ry, or Rz
                   false, true,          // rudder or throttle
                   false, true, false);  // accelerator, brake, or steering

Pedal brake = Pedal(A3, "B:");
Pedal throttle = Pedal(A0, "T:");
Pedal clutch = Pedal(A1, "C:");

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  loadEEPROMSettings();
  Joystick.begin();
  Joystick.setThrottle(0);
  Joystick.setBrake(0);
  Joystick.setZAxis(0);

  delay(2000);
}

// the loop routine runs over and over again forever:
void loop() {
  //  timing
  //  unsigned long start = micros(); //4492 microseconds
  brake.readValues();
  throttle.readValues();
  clutch.readValues();

  if (Serial.available() > 0) {
    String msg = Serial.readStringUntil('\n');
    String cm = ",";
    String dash = "-";

    resetDevice(msg, cm, dash);
    getMap(msg, cm, dash);
    getInverted(msg, cm, dash);
    getCalibration(msg, cm, dash);

    if (msg.indexOf("CMAP:") >= 0) {
      String cmap = msg;
      cmap.replace("CMAP:", "");
      clutch.setOutputMapValues(cmap, E_CLUTCH);
    }

    if (msg.indexOf("BMAP:") >= 0) {
      String bmap = msg;
      bmap.replace("BMAP:", "");
      brake.setOutputMapValues(bmap, E_BRAKE);
    }

    if (msg.indexOf("TMAP:") >= 0) {
      String tmap = msg;
      tmap.replace("TMAP:", "");
      throttle.setOutputMapValues(tmap, E_THROTTLE);
    }

    if (msg.indexOf("TCALI:") >= 0) {
      String tmap = msg;
      tmap.replace("TCALI:", "");
      throttle.setCalibrationValues(tmap, E_CALIBRATION_T);
    }
    if (msg.indexOf("BCALI:") >= 0) {
      String tmap = msg;
      tmap.replace("BCALI:", "");
      brake.setCalibrationValues(tmap, E_CALIBRATION_B);
    }
    if (msg.indexOf("CCALI:") >= 0) {
      String tmap = msg;
      tmap.replace("CCALI:", "");
      brake.setCalibrationValues(tmap, E_CALIBRATION_C);
    }

    updateInverted(msg);
  }


  Joystick.setThrottle(throttle.getAfterHID());
  Joystick.setBrake(brake.getAfterHID());
  Joystick.setZAxis(clutch.getAfterHID());
  Joystick.sendState(); // Update the Joystick status on the PC

  if (Serial.availableForWrite ()) {
    Serial.println(throttle.getPedalString() + brake.getPedalString() + clutch.getPedalString());
  }

  //  delay(300);
}
//---------------------------------------------------------

void loadEEPROMSettings() {
  if (EEPROM.read(E_INIT) == 'T') {
    loadDeviceSettings();
  } else {
    resetDeviceSettings();
  }
}

void loadDeviceSettings() {
  clutch.getEEPROMOutputMapValues(E_CLUTCH);
  brake.getEEPROMOutputMapValues(E_BRAKE);
  throttle.getEEPROMOutputMapValues(E_THROTTLE);


  String EEPROM_InvertedMap = utilLib.readStringFromEEPROM(E_PEDAL_INVERTED_MAP);
  String INVER = "INVER:";
  updateInverted(INVER + EEPROM_InvertedMap);


  clutch.getEEPROMOutputMapValues(E_CALIBRATION_C);
  brake.getEEPROMOutputMapValues(E_CALIBRATION_B);
  throttle.getEEPROMOutputMapValues(E_CALIBRATION_T);

}

void resetDeviceSettings() {
  // write
  EEPROM.write(E_INIT, 'T');

  clutch.resetOutputMapValues(E_CLUTCH);
  brake.resetOutputMapValues(E_THROTTLE);
  throttle.resetOutputMapValues(E_BRAKE);

  // 0 = false / 1 = true
  utilLib.writeStringToEEPROM(E_PEDAL_INVERTED_MAP, "0-0-0");

  clutch.resetCalibrationValues(E_CALIBRATION_C);
  brake.resetCalibrationValues(E_CALIBRATION_B);
  throttle.resetCalibrationValues(E_CALIBRATION_T);

  softwareReset::standard();
}

void resetDevice (String msg,  String cm, String dash) {
  if (msg.indexOf("ResetDevice") >= 0) {
    resetDeviceSettings();
  }
}

void getMap (String msg,  String cm, String dash) {
  if (msg.indexOf("GetMap") >= 0) {
    Serial.println(throttle.getOutputMapValues("TMAP:") + cm + brake.getOutputMapValues("BMAP:") + cm + clutch.getOutputMapValues("CMAP:"));
  }
}

void getInverted (String msg,  String cm, String dash) {
  if (msg.indexOf("GetInverted") >= 0) {
    String INVER = "INVER:";
    Serial.println(INVER + throttle.getInvertedValues() + dash + brake.getInvertedValues() + dash + clutch.getInvertedValues());
  }
}

void getCalibration (String msg,  String cm, String dash) {
  if (msg.indexOf("GetCali") >= 0) {
    String cm = ",";
    Serial.println(throttle.getCalibrationValues("TCALI:") + cm + brake.getCalibrationValues("BCALI:") + cm + clutch.getCalibrationValues("CCALI:"));
  }
}


void updateInverted (String msg) {
  if (msg.indexOf("INVER:") >= 0) {
    String splitINVER = utilLib.getValue(msg, ',', 0);
    splitINVER.replace("INVER:", "");
    throttle.setInvertedValues(utilLib.getValue(splitINVER, '-', 0).toInt());
    brake.setInvertedValues(utilLib.getValue(splitINVER, '-', 1).toInt());
    clutch.setInvertedValues(utilLib.getValue(splitINVER, '-', 2).toInt());

    utilLib.writeStringToEEPROM(E_PEDAL_INVERTED_MAP, splitINVER);
  }
}
