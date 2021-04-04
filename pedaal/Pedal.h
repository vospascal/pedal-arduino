#define SENSOR_RANGE 1023
#define SERIAL_RANGE 100

#ifndef UtilLib
#include "UtilLibrary.h"

// init util library
UtilLib utilLib;

class Pedal
{

  public:
    //initialise pedal
    Pedal(byte analogInput, String prefix)
    {
      _analogInput = analogInput;
      _prefix = prefix;
    }

    int Pedal::getAfterHID() {
      return _afterHID;
    }

    String Pedal::getPedalString() {
      return _pedalString;
    }

    void Pedal::readValues() {
      int rawValue = analogRead(_analogInput);
      Pedal::updatePedal(rawValue);
    }


    void Pedal::setInvertedValues(int invertedValues) {
      _inverted = invertedValues;
    }

    int Pedal::getInvertedValues() {
      return _inverted;
    }

    ////////////////////
    void Pedal::resetCalibrationValues(int EEPROMSpace) {
      int resetMap[4] = {0, SENSOR_RANGE, 0, SENSOR_RANGE};
      utilLib.writeStringToEEPROM(EEPROMSpace, utilLib.generateStringMapCali(resetMap));
    }

    void Pedal::getEEPROMCalibrationValues(int EEPROMSpace) {
      String EEPROM_Map = utilLib.readStringFromEEPROM(EEPROMSpace);
      Pedal::setCalibrationValues(EEPROM_Map, EEPROMSpace);
    }

    void Pedal::setCalibrationValues(String map, int EEPROMSpace) {
      _calibration[0] = utilLib.getValue(map, '-', 0).toInt();
      _calibration[1] = utilLib.getValue(map, '-', 1).toInt();
      _calibration[2] = utilLib.getValue(map, '-', 2).toInt();
      _calibration[3] = utilLib.getValue(map, '-', 3).toInt();

      // update EEPROM settings
      // todo:fix
      utilLib.writeStringToEEPROM(EEPROMSpace, map);
    }

    String Pedal::getCalibrationValues(String prefix) {
      return prefix + utilLib.generateStringMapCali(_calibration);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////

    void Pedal::getEEPROMOutputMapValues(int EEPROMSpace) {
      String EEPROM_Map = utilLib.readStringFromEEPROM(EEPROMSpace);
      Pedal::setOutputMapValues(EEPROM_Map, EEPROMSpace);
    }

    void Pedal::setOutputMapValues(String map, int EEPROMSpace) {
      _outputMap[0] = utilLib.getValue(map, '-', 0).toInt();
      _outputMap[1] = utilLib.getValue(map, '-', 1).toInt();
      _outputMap[2] = utilLib.getValue(map, '-', 2).toInt();
      _outputMap[3] = utilLib.getValue(map, '-', 3).toInt();
      _outputMap[4] = utilLib.getValue(map, '-', 4).toInt();
      _outputMap[5] = utilLib.getValue(map, '-', 5).toInt();

      // update EEPROM settings
      // todo:fix
      utilLib.writeStringToEEPROM(EEPROMSpace, map);
    }

    String Pedal::getOutputMapValues(String prefix) {
      return prefix + utilLib.generateStringMap(_outputMap);
    }

    void Pedal::resetOutputMapValues(int EEPROMSpace) {
      int resetMap[6] = {0, 20, 40, 60, 80, 100};
      utilLib.writeStringToEEPROM(EEPROMSpace, utilLib.generateStringMap(resetMap));
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////

  private:
    byte _analogInput;
    String _prefix;
    String _pedalString;
    int _afterHID;
    int _inverted = 0; //0 = false / 1 - true
    int _inputMap[6] =  { 0, 20, 40, 60, 80, 100 };
    int _outputMap[6] = { 0, 20, 40, 60, 80, 100 };
    int _calibration[4] = {0, SENSOR_RANGE, 0, SENSOR_RANGE}; // calibration low, calibration high, deadzone low, deadzone high


    void updatePedal(int rawValue) {
      int beforeSerial;
      int afterSerial;
      int pedalRaw;
      int beforeHID;
      int afterHID;

      ////////////////////////////////////////////////////////////////////////////////

      if (_inverted == 1) {
        rawValue = SENSOR_RANGE - rawValue;
      }

      int pedalOutput;
      int lowDeadzone = (_calibration[0] > _calibration[2]) ? _calibration[0] : _calibration[2];
      int topDeadzone = (_calibration[1] < _calibration[3]) ? _calibration[1] : _calibration[3];

      if (rawValue > topDeadzone) {
        pedalOutput = topDeadzone;
      } else if (rawValue < lowDeadzone) {
        pedalOutput = lowDeadzone;
      } else {
        pedalOutput = rawValue;
      }

      float inputMapHID[6] = {};
      utilLib.copyArray(_inputMap, inputMapHID, 6);
      utilLib.arrayMapMultiplier(inputMapHID, (SENSOR_RANGE / 100));

      float outputMapHID[6] = {};
      utilLib.copyArray(_outputMap, outputMapHID, 6);
      utilLib.arrayMapMultiplier(outputMapHID, (SENSOR_RANGE / 100));

      //map(value, fromLow, fromHigh, toLow, toHigh)
      beforeHID = map(pedalOutput, lowDeadzone, topDeadzone, 0, SENSOR_RANGE); // this upscales 500 -> 1023
      afterHID = multiMap<float>(beforeHID, inputMapHID, outputMapHID, 6);

      beforeSerial = map(pedalOutput, lowDeadzone, topDeadzone, 0, SERIAL_RANGE); // this downscales 500 -> 100
      afterSerial = multiMap<int>(beforeSerial, _inputMap, _outputMap, 6);

      ////////////////////////////////////////////////////////////////////////////////

      String p1 = ";";
      String cm = ",";
      String stringPrefix = _prefix;
      String stringValues = beforeSerial + p1 + afterSerial + p1 + rawValue + p1 + beforeHID + cm;
      String pedalString = stringPrefix + stringValues;

      _pedalString = pedalString;
      _afterHID = afterHID;
    }

};

#endif
