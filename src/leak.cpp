#include <Particle.h>

#define LEAK_PIN A0
#define CHECK_EVERY 1000

// < 3000 was seen in testing when wet
#define LEAK_ALERT_LEVEL 3200
#define LEAK_REPEATS 3

int leak_level;
int leak_reading_cnt = 0;

bool leak_alert_sent = false;

system_tick_t last_checked = 0;

int is_connected(String _unused);
int sendLeakAlert(String _unused);
int resetLeakAlert(String _unused);


void leak_setup() {
    pinMode(LEAK_PIN, INPUT_PULLUP);

    Particle.function("lk_rst_alert", resetLeakAlert);
    Particle.function("lk_snd_alert", sendLeakAlert);
    Particle.variable("lk_lvl", leak_level);
    Particle.variable("lk_snt", leak_alert_sent);
    Particle.variable("lk_rding_cnt", leak_reading_cnt);
}


void leak_loop() {
    // Only send one alert:
    if (leak_alert_sent) {
        return;
    }

    // Only check leak alerts periodically
    if (millis() - last_checked < CHECK_EVERY) {
        return;
    } else {
        last_checked = millis();
    }

    leak_level = analogRead(LEAK_PIN);
    if (leak_level > LEAK_ALERT_LEVEL) {
        // No Leak
        leak_reading_cnt = 0;
        return;
    } else {
        leak_reading_cnt++;
    }

    if (leak_reading_cnt >= LEAK_REPEATS) {
        sendLeakAlert("");
    }
}


int sendLeakAlert(String _unused) {
    if (leak_alert_sent == false) {
        leak_alert_sent = true;
        Particle.publish("alert/leak/boiler", PRIVATE);
    }
    return 0;
}


int resetLeakAlert(String _unused) {
    leak_alert_sent = false;
    return 0;
}
