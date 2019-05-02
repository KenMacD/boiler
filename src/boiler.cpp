/* Boiler controller */

#include <application.h>

#define ZONES 5
#define TEMP_PREFIX "temperature/"

SerialLogHandler logHandler(LOG_LEVEL_INFO, {
  { "comm.protocol", LOG_LEVEL_WARN},
  { "app", LOG_LEVEL_ALL }
});

#include "leak.h"
#include "zone.h"

Zone *zones[ZONES];
void handle_temperature(const char *, const char *);

void setup_zone_ports() {
   pinMode(D8, OUTPUT);
   pinMode(D7, OUTPUT);
   pinMode(D6, OUTPUT);
   pinMode(D5, OUTPUT);
   pinMode(D4, OUTPUT);
   digitalWrite(D8, HIGH);
   digitalWrite(D7, HIGH);
   digitalWrite(D6, HIGH);
   digitalWrite(D5, HIGH);
   digitalWrite(D4, HIGH);

}
STARTUP( setup_zone_ports() );


void setup() {
    leak_setup();


    // TODO: make these pins right
    zones[0] = new Zone("fmrm", D8);
    zones[1] = new Zone("lvrm", D7);
    zones[2] = new Zone("bdrm", D6);
    zones[3] = new Zone("mstr", D5);
    zones[4] = new Zone("bsmt", D4);

    Particle.subscribe(TEMP_PREFIX, handle_temperature, MY_DEVICES);
    Mesh.subscribe(TEMP_PREFIX, handle_temperature);
}


void loop() {
    leak_loop();

    for (int i = 0; i < ZONES; i++) {
        zones[i]->loop();
    }
}


void handle_temperature(const char *const_event, const char *data) {
    Log.trace("handle_temperature event %s - %s", const_event, data);

    char *event = strdup(const_event);
    strtok(event, "/"); // temperature

    String zone_name = strtok(NULL, "/");
    String verb = strtok(NULL, "/");
    free(event);

    // temperature/ZONE/is  - TEMP
    // temperature/ZONE/target - TEMP

    Zone *zone = NULL;
    Log.trace("handle_temperature zone named %s", zone_name.c_str());
    for (int i = 0; i < ZONES; i++) {
        if (strncmp(zones[i]->name(), zone_name.c_str(), 4) == 0) {
            Log.trace("handle_temperature zone matched %d", i);
            zone = zones[i];
        }
    }
    if (zone == NULL) {
        Particle.publish("boiler/exception/bad_zone", zone_name, PRIVATE);
    }

    float temp = strtof(data, NULL);

    if (verb == "is") {
        Log.trace("handle_temperature set temp to %f", temp);
        zone->set_current_temp(temp);
    } else if (verb == "set") {
        Log.trace("handle_temperature set target to %f", temp);
        zone->set_target_temp(temp);
    }
}
