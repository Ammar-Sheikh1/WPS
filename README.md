# Wifi Positioning System

"Wips" is my project based on Wi-Fi Indoor Positioning. The goal is to locate onself through the use of Wi-Fi Access Points. I originally intended to use the 802.11 Fine Timing Measurement Protocol in order to trilaterate my position. However, I currently face hardware setbacks, so for now this will be put on hold.
Note: I have not uploaded any APData produced while wardriving.


## Files
+ include/ - holds ammar-lib c++/.h files used for this project
+ wips.cc - Wifi Positioning
+ db_script - Parses netxml file and places info in DB

## Tools & Libraries used
+ Kismet - Used for "Wardriving", captured beacon frames and recorded gps info
+ leveldb - Key-Value DB, used to mapped Access Point BSSID to lat/long
+ sdbus-cpp - C++ binding for sdbus/dbus which allows for IPC with NetworkManager Daemon

## Todo
+ RSSI Positioning and trilateration

# Access Points
Accurately collecting access points is a challenge...
![alt text](https://github.com/Ammar-Sheikh1/WPS/blob/master/pics/SpeedwayAPs.png)
![alt text](https://github.com/Ammar-Sheikh1/WPS/blob/master/pics/MidCampus.png)






