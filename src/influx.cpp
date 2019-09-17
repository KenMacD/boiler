#include "influx.h"
#include "config.h"

static UDP udp;
static const uint8_t remoteAddr[33] = INFLUX_IP6
static int port = INFLUX_PORT;

static const IPAddress remoteIP((const HAL_IPAddress&)remoteAddr);

static Zone **zones;
static int zone_cnt;
static system_tick_t last_update;

void influx_setup(Zone **_zones, int _zone_cnt) {
    udp.begin(0);
    zones = _zones;
    zone_cnt = _zone_cnt;
    last_update = millis();
}

void influx_loop() {
    system_tick_t now = millis();
    if (now - last_update < 5 * 1000UL){
        return;
    } else {
        last_update = now;
    }

    Zone *zone;
    char buffer[128];

    Serial.println("Publishing zones!");
    for (int i = 0; i < zone_cnt; i++) {
        zone = zones[i];
        snprintf(buffer, sizeof(buffer), "heat,zone=%s heating=%s,goal=%.2f",
                zone->name(), zone->is_heating()?"t":"f", zone->target_temp());

        if (udp.sendPacket(buffer, strlen(buffer), remoteIP, port) < 0) {
            Serial.println("Bad sendPacket");
            Particle.publish("boiler/exception/UDPbad", PRIVATE);
        } else {
            Serial.println("Good sendPacket");
        }
    }
  }
