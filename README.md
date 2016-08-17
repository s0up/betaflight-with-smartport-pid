Install betaflight to board

Copy LUA file to /SCRIPTS/«modelname»/telemXX.lua (on old taranis versions) - For telemetry scripts
COPY LUA file to /SCRIPTS/TELEMETRY/telemXX.lua (on newer taranis versions) 

Go to telemetry and set Screen 1 to "Script" and set the script as telemXX (whatever number you chose)

Configure your ranges in cleanflight for PID:

adjrange 0 0 4 1425 1575 0 5
adjrange 1 0 4 1350 1400 6 5
adjrange 2 0 4 1275 1325 7 5
adjrange 3 0 4 1200 1250 8 5
adjrange 4 0 4 1125 1175 9 5
adjrange 5 0 4 1050 1100 10 5
adjrange 6 0 4 900 1025 11 5
adjrange 7 0 4 1600 1650 4 5
adjrange 8 0 4 1675 1725 5 5
adjrange 9 0 4 1750 1800 1 5
adjrange 10 0 4 1825 1875 2 5
adjrange 11 0 4 1900 1950 3 5
adjrange 12 0 4 1975 2100 0 5
adjrange 13 0 0 900 900 0 0
adjrange 14 0 0 900 900 0 0


