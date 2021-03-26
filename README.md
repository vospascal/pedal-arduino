## pedal-gui-arduino
arduino side to client gui found here https://github.com/vospascal/pedal-gui

## help
If you want to develop the software furter please create a pull request or create an issue

## if you like it
please leave a star

## if you realy like it
if you like it you can donate further developent
https://www.paypal.com/donate?business=TBPE6XCB2XBMW&item_name=pedalbox&currency_code=EUR

add this to your arduino boards.text

location example  [yourinstallationdirectory]\arduino-1.8.13\hardware\arduino\avr\boards.txt
```
##############################################################

micro.name=PedalBox

micro.vid.0=0x2341
micro.pid.0=0x0037
micro.vid.1=0x2341
micro.pid.1=0x8037
micro.vid.2=0x2A03
micro.pid.2=0x0037
micro.vid.3=0x2A03
micro.pid.3=0x8037
micro.vid.4=0x2341
micro.pid.4=0x0237
micro.vid.5=0x2341
micro.pid.5=0x8237

micro.upload.tool=avrdude
micro.upload.protocol=avr109
micro.upload.maximum_size=28672
micro.upload.maximum_data_size=2560
micro.upload.speed=57600
micro.upload.disable_flushing=true
micro.upload.use_1200bps_touch=true
micro.upload.wait_for_upload_port=true

micro.bootloader.tool=avrdude
micro.bootloader.low_fuses=0xff
micro.bootloader.high_fuses=0xd8
micro.bootloader.extended_fuses=0xcb
micro.bootloader.file=caterina/Caterina-Micro.hex
micro.bootloader.unlock_bits=0x3F
micro.bootloader.lock_bits=0x2F

micro.build.mcu=atmega32u4
micro.build.f_cpu=16000000L
micro.build.vid=0x2341
micro.build.pid=0x8037
micro.build.usb_product="PedalBox"
micro.build.board=AVR_MICRO
micro.build.core=arduino
micro.build.variant=micro
micro.build.extra_flags={build.usb_flags}

##############################################################
```


# pedal-arduino
it reads 0 - 5 volt scale. So as long as its within that range it'll work.

Measure the current, use "Arduino Micro Pro" as to read the analogue value from the sensor

## conectors pedal side
1 V5/vcc
5 GND
6 analog/a1

2 V5/vcc
3 GND
4 analog/a1
