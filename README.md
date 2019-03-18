

# Boiler

A Particle project for controlling my 5 zone boiler.

## Connections

 * D8 - D4: Relays for zone valves
 * A0: Wire leak sensor

## Modules

 The code is split up in to separate modules to handle the above connections. Each one has its own setup and loop.

 * zone: handle one heating zone
 * leak: raise an alert if the resistance between two wires drops
 * tinker: Particle's tinker code, to allow manual flipping of pins

## Useful commands

Compile cloud: `particle compile --saveTo boiler.bin xenon`
Flash local: `particle flash --serial boiler`
Flash cloud: `particle flash boiler boiler.bin`
Monitor serial: `particle serial monitor`
Publish temperature for testing: `particle publish "temperature/bdrm/is" "25.0"`
