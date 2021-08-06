# ESP01_MQTT_DTH
This project is my first attempt to create a Smart Home Sensor,  most of the code and the knowledge from Dr. Andreas Spiess.  The hardware is  ESP-01 and DTH .


The MQTT message is look like this:
if all is ok
{"Sensor":"DTH1","Msg#":251,"Bat":0.106452,"Status ":"ok","data":[28.3,60.6]}
and if there is sensor error reading


note:
The "Bat" is not working :(  (it need to be battary voltage lavel)



The Server is based on Dr. Andreas Spiess youtube: https://youtu.be/KJRMjUzlHI8 
https://github.com/SensorsIot/Raspberry-Pi-Server
