Was playing around with 3-D printing, Photon, and MQTT; and thought of creating an MQTT-enabled counter. And how could it be fun, without some rainbow blinking LEDs and FASTLED.

The counter has been designed to count up-to 7 as it is intended to be a day calculator; but can easily go up to 9 if required.

simply publish 'INC' (to increase) and 'DEC' (to decrease) to the channel 'photon1/weeklycounter'. Of course - you will have to provide the address for your MQTT broker. 

Am especially proud of how I handled the segments using bit-level mapping. 


Small Demo:

https://goo.gl/photos/N2YnqvTqu1gVm6LS7
