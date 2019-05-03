
#include "zone.h"

#define HYSTERESIS 0.3
#define MIN_TEMP 5.0
#define MIN_ON_TIME (30 * 1000UL)
#define MIN_OFF_TIME (30 * 1000UL)
#define ON 1
#define OFF 0
#define UPDATE_NEED (10 * 60 * 1000UL)


// TODO: handle boot temperature
Zone::Zone(String name, int pin, void (*block_updates)(bool))
    : m_pin(pin),
      m_block_updates(block_updates),

      m_current_temp(MIN_TEMP),
      m_last_current_temp(millis()),

      m_target_temp(MIN_TEMP),

      m_heating(false),
      m_last_state_change(millis())
{
  strlcpy(m_name, name.c_str(), sizeof(m_name));

  pinMode(m_pin, OUTPUT);
  digitalWrite(m_pin, HIGH);

  Particle.variable((m_name + String("_heat")).c_str(), &m_heating, BOOLEAN);
  Particle.variable((m_name + String("_temp")).c_str(), &m_current_temp, DOUBLE);
  Particle.variable((m_name + String("_goal")).c_str(), &m_target_temp, DOUBLE);
  Particle.function((m_name + String("_set")).c_str(), &Zone::set_target_temp_cloud, this);
  Particle.publish("variable-registered", (String("zone-") + m_name).c_str(), PRIVATE);
}


Zone::~Zone() {
  digitalWrite(m_pin, HIGH);
}


void Zone::loop() {
  system_tick_t now, time_since_change, time_since_update;
  SINGLE_THREADED_BLOCK() {
    now = millis();
    time_since_change = now - m_last_state_change;
    time_since_update = now - m_last_current_temp;
  }

  if (time_since_change < (m_heating ? MIN_ON_TIME : MIN_OFF_TIME)) {
    return;
  }

  if (time_since_update > UPDATE_NEED) {
    if (m_heating) {
      Log.info("Zone %s not reporting, turning heat off", m_name);
      Particle.publish(String::format("boiler/%s/timeout", m_name),
        String::format("%d > %d", time_since_update, UPDATE_NEED), PRIVATE);
      turn_heat(OFF);
    }
    return;
  }

  if (m_heating) {
    if (m_current_temp > m_target_temp + HYSTERESIS) {
      Log.info("Zone %s turning heat off", m_name);
      turn_heat(OFF);
    }
  }
  else if (m_current_temp < m_target_temp - HYSTERESIS) {
    Log.info("Zone %s turning heat on.", m_name);
    turn_heat(ON);
  }
}


void Zone::turn_heat(bool on) {
  const char *on_str;
  if (on) {
    on_str = "on";
  } else  {
    on_str = "off";
  }
  m_last_state_change = millis();
  m_block_updates(on);
  Particle.publish(String::format("boiler/%s/heating", m_name),
              on_str, PRIVATE);
  Log.trace("Zone %s writing %d to pin %d", m_name, !on, m_pin);
  digitalWrite(m_pin, !on); // Low activates
  m_heating = on;
}


void Zone::set_current_temp(float temperature) {
  // TODO: monitor overshoot
  Log.trace("Zone %s update current temperature %f", m_name, temperature);
  m_last_current_temp = millis();
  m_current_temp = temperature;
}


int Zone::set_target_temp(float temperature) {
  Log.trace("Zone %s update target temperature %f", m_name, temperature);
  m_target_temp = temperature;
  // TODO: publish message?
  return 0;
}

int Zone::set_target_temp_cloud(String temperature) {
    float temp = temperature.toFloat();
    if (temp == 0) {
        return -1;
    }
    return set_target_temp(temp);
}
