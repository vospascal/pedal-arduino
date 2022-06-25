/*
  Joystick.h

  Copyright (c) 2015-2017, Matthew Heironimus

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef JOYSTICK_h
#define JOYSTICK_h

#include "DynamicHID/DynamicHID.h"

#if ARDUINO < 10606
#error The Joystick library requires Arduino IDE 1.6.6 or greater. Please update your IDE.
#endif // ARDUINO < 10606

#if ARDUINO > 10606
#if !defined(USBCON)
#error The Joystick library can only be used with a USB MCU (e.g. Arduino Leonardo, Arduino Micro, etc.).
#endif // !defined(USBCON)
#endif // ARDUINO > 10606

#if !defined(_USING_DYNAMIC_HID)

#warning "Using legacy HID core (non pluggable)"

#else // !defined(_USING_DYNAMIC_HID)

//================================================================================
//  Joystick (Gamepad)

#define JOYSTICK_DEFAULT_REPORT_ID         0x03
#define JOYSTICK_DEFAULT_AXIS_MINIMUM         0
#define JOYSTICK_DEFAULT_AXIS_MAXIMUM      1023
#define JOYSTICK_TYPE_JOYSTICK             0x04
#define JOYSTICK_TYPE_GAMEPAD              0x05
#define JOYSTICK_TYPE_MULTI_AXIS           0x08

class Joystick_
{
private:

    // Joystick State
    int32_t   _xAxisRotation;
    int32_t   _yAxisRotation;
    int32_t   _zAxisRotation;

    // Joystick Settings
    bool     _autoSendState;
    uint8_t  _includeAxisFlags;
    int32_t  _rxAxisMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int32_t  _rxAxisMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int32_t  _ryAxisMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int32_t  _ryAxisMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;
    int32_t  _rzAxisMinimum = JOYSTICK_DEFAULT_AXIS_MINIMUM;
    int32_t  _rzAxisMaximum = JOYSTICK_DEFAULT_AXIS_MAXIMUM;

    uint8_t   _hidReportId;
    uint8_t   _hidReportSize;

protected:
    int buildAndSet16BitValue(bool includeValue, int32_t value, int32_t valueMinimum, int32_t valueMaximum, int32_t actualMinimum, int32_t actualMaximum, uint8_t dataLocation[]);
    int buildAndSetAxisValue(bool includeAxis, int32_t axisValue, int32_t axisMinimum, int32_t axisMaximum, uint8_t dataLocation[]);

public:
    Joystick_(
        uint8_t hidReportId = JOYSTICK_DEFAULT_REPORT_ID,
        uint8_t joystickType = JOYSTICK_TYPE_JOYSTICK,
        bool includeRxAxis = true,
        bool includeRyAxis = true,
        bool includeRzAxis = true
        );

    void begin(bool initAutoSendState = true);
    void end();

    // Set Range Functions
    inline void setRxAxisRange(int32_t minimum, int32_t maximum)
    {
        _rxAxisMinimum = minimum;
        _rxAxisMaximum = maximum;
    }
    inline void setRyAxisRange(int32_t minimum, int32_t maximum)
    {
        _ryAxisMinimum = minimum;
        _ryAxisMaximum = maximum;
    }
    inline void setRzAxisRange(int32_t minimum, int32_t maximum)
    {
        _rzAxisMinimum = minimum;
        _rzAxisMaximum = maximum;
    }

    // Set Axis Values
    void setRxAxis(int32_t value);
    void setRyAxis(int32_t value);
    void setRzAxis(int32_t value);

    void sendState();
};

#endif // !defined(_USING_DYNAMIC_HID)
#endif // JOYSTICK_h