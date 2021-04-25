#define SENSOR_RANGE 1023
#define SERIAL_RANGE 100

#define BRAKE_PEDAL_LOAD_BEAM_CELL_TARE_REPS  10
#define BRAKE_PEDAL_LOAD_BEAM_CELL_MAX_VAL  2000000
#define BRAKE_PEDAL_LOAD_BEAM_CELL_SCALING  1100
#define SENSITIVITY 64 // Setting for HX711 Load Cell sensitivity. Medium = 64, High = 128, (*Channel B only, Low = 32)
//#define GAIN 128

#ifndef UtilLib
#include "UtilLibrary.h"

#include "Smoothed.h"

#include <HX711.h>

#include "ADS1X15.h"

// init util library
UtilLib utilLib;


class Pedal
{

  public:
    //initialise pedal
    Pedal(String prefix) {
      _prefix = prefix;
      _mySensor.begin(SMOOTHED_EXPONENTIAL, 10);
      _mySensor.clear();
    }

    void Pedal::ConfigAnalog ( byte analogInput) {
      _analogInput = analogInput;
      _signal = 0;
    }

    void Pedal::ConfigLoadCell (int DOUT, int CLK)
    {
      HX711 _loadCell(DOUT, CLK, 128);
      _loadCell.tare(BRAKE_PEDAL_LOAD_BEAM_CELL_TARE_REPS); // Reset values to zero
      _signal = 1;
    }

    void Pedal::ConfigADS (int channel)
    {
      ADS1115 _ads1015;
      _ads1015.begin();
      _ads1015.setGain(0);      // 6.144 volt
      _ads1015.setDataRate(7);  // fast
      _ads1015.setMode(0);      // continuous mode
      _ads1015.readADC(channel);      // first read to trigger
      _signal = 2;
    }

    int Pedal::getAfterHID() {
      return _afterHID;
    }

    String Pedal::getPedalString() {
      return _pedalString;
    }

    void Pedal::readValues() {
      int rawValue = 0;
      if (_signal == 0) {
        rawValue = analogRead(_analogInput);
        if (rawValue < 0) rawValue = 0;
      }
      if (_signal == 1) {
        rawValue = _loadCell.read();
        if (rawValue > BRAKE_PEDAL_LOAD_BEAM_CELL_MAX_VAL) {
          rawValue = 0;
        }
        if (rawValue < 0) rawValue = 0;
        rawValue /= BRAKE_PEDAL_LOAD_BEAM_CELL_SCALING;
      }
      if (_signal == 2) {
        rawValue = _ads1015.getValue();
        if (rawValue < 0) rawValue = 0;
      }

      Pedal::updatePedal(rawValue);
    }

    void Pedal::setSmoothValues(int smoothValues) {
      _smooth = smoothValues;
    }

    int Pedal::getSmoothValues() {
      return _smooth;
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
      _calibration[0] = resetMap[0];
      _calibration[1] = resetMap[1];
      _calibration[2] = resetMap[2];
      _calibration[3] = resetMap[3];
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
    String _prefix;
    String _pedalString;
    int _afterHID;
    int _signal = 0;
    Smoothed <int> _mySensor;
    HX711 _loadCell;
    ADS1115 _ads1015;
    int _analogInput = 0;
    int _inverted = 0; //0 = false / 1 - true
    int _smooth = 0; //0 = false / 1 - true
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

      if (_smooth == 1) {
        _mySensor.add(rawValue);
        rawValue = _mySensor.get();
      }

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
