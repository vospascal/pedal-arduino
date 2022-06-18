#ifndef Pedals_h
#define Pedals_h

#include "src/Joystick/Joystick.h"
#include "src/MultiMap/MultiMap.h"
#include "src/SoftwareReset/SoftwareReset.h"
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
#define E_PEDAL_SMOOTH_MAP 210
#define SERIAL_RANGE 100

String cm = ",";
String dash = "-";

ADS1115 _ads1015;

Joystick_ _joystick(
  JOYSTICK_DEFAULT_REPORT_ID,
  JOYSTICK_TYPE_GAMEPAD,
  0, 0,                 // Button Count, Hat Switch Count
  false, false, false,  // X and Y, Z Axis
  true, true, true,     // Rx, Ry, or Rz
  false, false,         // rudder or throttle
  false, false, false   // accelerator, brake, or steering
); 

// create the pedals
Pedal _throttle = Pedal("T:");
Pedal _brake = Pedal("B:");
Pedal _clutch = Pedal("C:");

class Pedals {
  public:
    //initialise pedal

    void Pedals::setup() {
      Pedals::loadEEPROMSettings();
      _joystick.begin();

      _ads1015.begin();
      _ads1015.setGain(0);      // 6.144 volt
      _ads1015.setDataRate(7);  // fast
      _ads1015.setMode(0);      // continuous mode

      _joystick.setRxAxis(0);
      _joystick.setRxAxisRange(0, (_throttle_hid_bit - 1));
      _throttle.setBits((_throttle_raw_bit - 1), (_throttle_hid_bit - 1));

      _joystick.setRyAxis(0);
      _joystick.setRyAxisRange(0, (_brake_hid_bit - 1));
      _brake.setBits((_brake_raw_bit - 1), (_brake_hid_bit - 1));

      _joystick.setRzAxis(0);
      _joystick.setRzAxisRange(0, (_clutch_hid_bit - 1));
      _clutch.setBits((_clutch_raw_bit - 1), (_clutch_hid_bit - 1));
    }

    void Pedals::loop() {
      if (Serial.available() > 0) {
        String msg = Serial.readStringUntil('\n');

        if (msg.indexOf("clearEEPROM") >= 0) {
          for (int i = 0; i < EEPROM.length(); i++) {
            EEPROM.write(i, 0);
          }
          Serial.println("done");
        }

        Pedals::resetDevice(msg);
        Pedals::getUsage(msg);
        Pedals::getMap(msg);
        Pedals::getInverted(msg);
        Pedals::getSmooth(msg);
        Pedals::getCalibration(msg);
        Pedals::getBits(msg);

        if (msg.indexOf("CMAP:") >= 0) {
          String cmap = msg;
          cmap.replace("CMAP:", "");
          _clutch.setOutputMapValues(cmap, E_CLUTCH);
        }

        if (msg.indexOf("BMAP:") >= 0) {
          String bmap = msg;
          bmap.replace("BMAP:", "");
          _brake.setOutputMapValues(bmap, E_BRAKE);
        }

        if (msg.indexOf("TMAP:") >= 0) {
          String tmap = msg;
          tmap.replace("TMAP:", "");
          _throttle.setOutputMapValues(tmap, E_THROTTLE);
        }

        if (msg.indexOf("CALIRESET") >= 0) {
          _clutch.resetCalibrationValues(E_CALIBRATION_C);
          _brake.resetCalibrationValues(E_CALIBRATION_B);
          _throttle.resetCalibrationValues(E_CALIBRATION_T);
        }

        if (msg.indexOf("CCALI:") >= 0 && msg.indexOf("BCALI:") >= 0 && msg.indexOf("TCALI:") >= 0) {
          String splitTCALI = utilLib.getValue(msg, ',', 0);
          splitTCALI.replace("TCALI:", "");
          _throttle.setCalibrationValues(splitTCALI, E_CALIBRATION_T);

          String splitBCALI = utilLib.getValue(msg, ',', 1);
          splitBCALI.replace("BCALI:", "");
          _brake.setCalibrationValues(splitBCALI, E_CALIBRATION_B);

          String splitCCALI = utilLib.getValue(msg, ',', 2);
          splitCCALI.replace("CCALI:", "");
          _clutch.setCalibrationValues(splitCCALI, E_CALIBRATION_C);
        }

        Pedals::updateInverted(msg);
        Pedals::updateSmooth(msg);
      }

      String SerialString = "";

      if(_throttle_on){
        _throttle.readValues();
        _joystick.setRxAxis(_throttle.getAfterHID());
          SerialString += _throttle.getPedalString();
      }
      
      if(_brake_on){
         _brake.readValues();
         _joystick.setRyAxis(_brake.getAfterHID());
         SerialString += _brake.getPedalString();
      }

      if(_clutch_on){
        _clutch.readValues();
        _joystick.setRzAxis(_clutch.getAfterHID());
        SerialString += _clutch.getPedalString();
      }

      _joystick.sendState(); // Update the Joystick status on the PC

      if (Serial.availableForWrite()) {
        Serial.println(SerialString);
      }
    }

    ///////////////////////// throttle /////////////////////////
    void Pedals::setThrottleOn(bool on) {
      _throttle_on = on;
    }

    void Pedals::setThrottleBits(String rawBit, String hidBit) {
      _throttle_raw_bit = Pedals::getBit(rawBit);
      _throttle_hid_bit = Pedals::getBit(hidBit);
    }

    void Pedals::setThrottleLoadcell(int DOUT, int CLK) {
      _throttle_pedalType = "Loadcell";
      _throttle.ConfigLoadCell(DOUT, CLK);
    }

    void Pedals::setThrottleADSChannel(int channel) {
      _throttle_pedalType = "ADS";
      _throttle.ConfigADS(_ads1015, channel);
    }

    void Pedals::setThrottleAnalogPin(int analogInput) {
      _throttle_pedalType = "Analog";
      _throttle_analog_input = analogInput;
      _throttle.ConfigAnalog(analogInput);
    }

    String Pedals::getThrottleType() {
      return _throttle_pedalType;
    }

    ///////////////////////// brake /////////////////////////
    void Pedals::setBrakeOn(bool on) {
      _brake_on = on;
    }

    void Pedals::setBrakeBits(String rawBit, String hidBit) {
      _brake_raw_bit = Pedals::getBit(rawBit);
      _brake_hid_bit = Pedals::getBit(hidBit);
    }

    void Pedals::setBrakeLoadcell(int DOUT, int CLK) {
      _brake_pedalType = "Loadcell";
      _brake.ConfigLoadCell(DOUT, CLK);
    }

    void Pedals::setBrakeADSChannel(int channel) {
      _brake_pedalType = "ADS";
      _brake.ConfigADS(_ads1015, channel);
    }

    void Pedals::setBrakeAnalogPin(int analogInput) {
      _brake_pedalType = "Analog";
      _brake_analog_input = analogInput;
      _brake.ConfigAnalog(analogInput);
    }

    String Pedals::getBrakeType() {
      return _brake_pedalType;
    }

    ///////////////////////// clutch /////////////////////////
    void Pedals::setClutchOn(bool on) {
      _clutch_on = on;
    }

    void Pedals::setClutchBits(String rawBit, String hidBit) {
      _clutch_raw_bit = Pedals::getBit(rawBit);
      _clutch_hid_bit = Pedals::getBit(hidBit);
    }

    void Pedals::setClutchLoadcell(int DOUT, int CLK) {
      _clutch_pedalType = "Loadcell";
      _clutch.ConfigLoadCell(DOUT, CLK);
    }

    void Pedals::setClutchADSChannel(int channel) {
      _clutch_pedalType = "ADS";
      _clutch.ConfigADS(_ads1015, channel);
    }

    void Pedals::setClutchAnalogPin(int analogInput) {
      _clutch_pedalType = "Analog";
      _clutch_analog_input = analogInput;
      _clutch.ConfigAnalog(analogInput);
    }

    String Pedals::getClutchType() {
      return _clutch_pedalType;
    }

  private:
    ////// throttle config //////
    bool _throttle_on = false;
    long _throttle_raw_bit = 65535; // default 16bit
    long _throttle_hid_bit = 65535; // default 16bit
    String _throttle_pedalType = "Analog"; //default Analog list: Analog, Loadcell, ADS
    byte _throttle_analog_input = A0; //default analog input

    ////// brake config //////
    bool _brake_on = false;
    long _brake_raw_bit = 65535; // default 16bit
    long _brake_hid_bit = 65535; // default 16bit
    String _brake_pedalType = "Analog"; //default Analog list: Analog, Loadcell, ADS
    byte _brake_analog_input = A0; //default analog input

    ////// clutch config //////
    bool _clutch_on = false;
    long _clutch_raw_bit = 65535; // default 16bit
    long _clutch_hid_bit = 65535; // default 16bit
    String _clutch_pedalType = "Analog"; //default Analog list: Analog, Loadcell, ADS
    byte _clutch_analog_input = A0; //default analog input

    long Pedals::getBit(String bits) {
      if (bits == "8bit") {
        return 255;
      }
      if (bits == "9bit") {
        return 511;
      }
      if (bits == "10bit") {
        return 1023 ;
      }
      if (bits == "11bit") {
        return 2047 ;
      }
      if (bits == "12bit") {
        return 4095 ;
      }
      if (bits == "13bit") {
        return 8191 ;
      }
      if (bits == "14bit") {
        return 16383 ;
      }
      if (bits == "15bit") {
        return 32767;
      }
      if (bits == "16bit") {
        return 65535;
      }
      if (bits == "17bit") {
        return 131071;
      }
      if (bits == "18bit") {
        return 262143;
      }
      if (bits == "19bit") {
        return 524287;
      }
      if (bits == "20bit") {
        return 1048575;
      }
      if (bits == "21bit") {
        return 2097151;
      }
      if (bits == "22bit") {
        return 4194303;
      }
      if (bits == "23bit") {
        return 8388607;
      }
      if (bits == "24bit") {
        return 16777215;
      }
      return 65535;
    }

    /////////////////////////////////////////////

    void Pedals::loadEEPROMSettings() {
      if (EEPROM.read(E_INIT) == 'T') {
        loadDeviceSettings();
      } else {
        resetDeviceSettings();
      }
    }

    void Pedals::loadDeviceSettings() {
      _clutch.getEEPROMOutputMapValues(E_CLUTCH);
      _brake.getEEPROMOutputMapValues(E_BRAKE);
      _throttle.getEEPROMOutputMapValues(E_THROTTLE);


      String EEPROM_InvertedMap = utilLib.readStringFromEEPROM(E_PEDAL_INVERTED_MAP);
      String INVER = "INVER:";
      updateInverted(INVER + EEPROM_InvertedMap);

      String EEPROM_SmoothMap = utilLib.readStringFromEEPROM(E_PEDAL_SMOOTH_MAP);
      String SMOOTH = "SMOOTH:";
      updateSmooth(SMOOTH + EEPROM_SmoothMap);


      _clutch.getEEPROMCalibrationValues(E_CALIBRATION_C);
      _brake.getEEPROMCalibrationValues(E_CALIBRATION_B);
      _throttle.getEEPROMCalibrationValues(E_CALIBRATION_T);

    }

    void Pedals::resetDeviceSettings() {
      // write
      EEPROM.write(E_INIT, 'T');

      _clutch.resetOutputMapValues(E_CLUTCH);
      _brake.resetOutputMapValues(E_THROTTLE);
      _throttle.resetOutputMapValues(E_BRAKE);

      // 0 = false / 1 = true
      utilLib.writeStringToEEPROM(E_PEDAL_INVERTED_MAP, "0-0-0");

      // 0 = false / 1 = true
      utilLib.writeStringToEEPROM(E_PEDAL_SMOOTH_MAP, "1-1-1");

      _clutch.resetCalibrationValues(E_CALIBRATION_C);
      _brake.resetCalibrationValues(E_CALIBRATION_B);
      _throttle.resetCalibrationValues(E_CALIBRATION_T);

      softwareReset::standard();
    }

    void Pedals::resetDevice(String msg) {
      if (msg.indexOf("ResetDevice") >= 0) {
        resetDeviceSettings();
      }
    }

    void Pedals::getUsage(String msg) {
      if (msg.indexOf("GetUsage") >= 0) {
        String USAGE = "USAGE:";
        Serial.println(USAGE + _throttle_on + dash + _brake_on + dash + _clutch_on);
      }
    }

    void Pedals::getMap(String msg) {
      if (msg.indexOf("GetMap") >= 0) {
        Serial.println(
            _throttle.getOutputMapValues("TMAP:") + cm +
            _brake.getOutputMapValues("BMAP:") + cm +
            _clutch.getOutputMapValues("CMAP:")
        );
      }
    }

    void Pedals::getSmooth(String msg) {
      if (msg.indexOf("GetSmooth") >= 0) {
        String SMOOTH = "SMOOTH:";
        Serial.println(
            SMOOTH +
            _throttle.getSmoothValues() + dash +
            _brake.getSmoothValues() + dash +
            _clutch.getSmoothValues()
       );
      }
    }

    void Pedals::getBits(String msg) {
      if (msg.indexOf("GetBits") >= 0) {
        String BITS = "BITS:";
        Serial.println(
            BITS +
            _throttle_raw_bit + dash + _throttle_hid_bit + dash +
            _brake_raw_bit + dash + _brake_hid_bit + dash +
            _clutch_raw_bit+ dash + _clutch_hid_bit
        );
      }
    }


    void Pedals::getInverted(String msg) {
      if (msg.indexOf("GetInverted") >= 0) {
        String INVER = "INVER:";
        Serial.println(
            INVER +
            _throttle.getInvertedValues() + dash +
            _brake.getInvertedValues() + dash +
            _clutch.getInvertedValues()
        );
      }
    }

    void Pedals::getCalibration(String msg) {
      if (msg.indexOf("GetCali") >= 0) {
        String cm = ",";
        Serial.println(
            _throttle.getCalibrationValues("TCALI:") + cm +
            _brake.getCalibrationValues("BCALI:") + cm +
            _clutch.getCalibrationValues("CCALI:")
        );
      }
    }

    void Pedals::updateSmooth(String msg) {
      if (msg.indexOf("SMOOTH:") >= 0) {
        String splitSMOOTH = utilLib.getValue(msg, ',', 0);
        splitSMOOTH.replace("SMOOTH:", "");
        _throttle.setSmoothValues(utilLib.getValue(splitSMOOTH, '-', 0).toInt());
        _brake.setSmoothValues(utilLib.getValue(splitSMOOTH, '-', 1).toInt());
        _clutch.setSmoothValues(utilLib.getValue(splitSMOOTH, '-', 2).toInt());

        utilLib.writeStringToEEPROM(E_PEDAL_SMOOTH_MAP, splitSMOOTH);
      }
    }

    void Pedals::updateInverted(String msg) {
      if (msg.indexOf("INVER:") >= 0) {
        String splitINVER = utilLib.getValue(msg, ',', 0);
        splitINVER.replace("INVER:", "");
        _throttle.setInvertedValues(utilLib.getValue(splitINVER, '-', 0).toInt());
        _brake.setInvertedValues(utilLib.getValue(splitINVER, '-', 1).toInt());
        _clutch.setInvertedValues(utilLib.getValue(splitINVER, '-', 2).toInt());

        utilLib.writeStringToEEPROM(E_PEDAL_INVERTED_MAP, splitINVER);
      }
    }

};
#endif
