Install betaflight to board

TARANIS CONFIG:

Copy LUA file to /SCRIPTS/«modelname»/telemXX.lua (on old taranis versions) - For telemetry scripts
COPY LUA file to /SCRIPTS/TELEMETRY/telemXX.lua (on newer taranis versions) 

Go to telemetry and set Screen 1 to "Script" and set the script as telemXX (whatever number you chose)

Setup AUX4 to the S2 knob
Setup AUX3 to the SC 3 position switch

CLEANFLIGHT CONFIG: 

Paste these ranges into betaflight CLI and save

# adjrange
adjrange 0 0 3 1000 1100 15 2

adjrange 1 0 3 1100 1200 16 2

adjrange 2 0 3 1200 1300 17 2

adjrange 3 0 3 1300 1400 18 2

adjrange 4 0 3 1400 1475 19 2

adjrange 5 0 3 1525 1600 20 2

adjrange 6 0 3 1600 1700 9 2

adjrange 7 0 3 1700 1800 10 2

adjrange 8 0 3 1800 1900 11 2

adjrange 9 0 3 1900 2000 1 2
