
#include "zone.h"

#define TEMP_RANGE 1.0
#define MIN_TEMP 5.0
#define MIN_ON_TIME (2 * 60 * 1000UL)
#define MIN_OFF_TIME (2 * 60 * 1000UL)
#define ON 1
#define OFF 0
#define UPDATE_NEED (10 * 60 * 1000UL)


// TODO: handle boot temperature
Zone::Zone(String name, int pin)
    : m_name(name),
      m_pin(pin),

      m_current_temp(MIN_TEMP),
      m_last_current_temp(millis()),

      m_target_temp(MIN_TEMP),

      m_heating(false),
      m_last_state_change(millis())
{
  pinMode(m_pin, OUTPUT);
  digitalWrite(m_pin, HIGH);
}


Zone::~Zone() {
  digitalWrite(m_pin, LOW);
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
      Log.info("Zone %s not reporting, turning heat off", m_name.c_str());
      turn_heat(OFF);
    }
    return;
  }

  if (m_heating) {
    if (m_current_temp > m_target_temp + TEMP_RANGE) {
      Log.info("Zone %s turning heat off", m_name.c_str());
      turn_heat(OFF);
    }
  }
  else if (m_current_temp < m_target_temp - TEMP_RANGE) {
    Log.info("Zone %s turning heat on.", m_name.c_str());
    turn_heat(ON);
  }
}


void Zone::turn_heat(bool on) {
  m_last_state_change = millis();
  Log.trace("Zone %s writing %d to pin %d", m_name.c_str(), !on, m_pin);
  digitalWrite(m_pin, !on); // Low activates
  m_heating = on;
  // TODO: publish message
}


void Zone::set_current_temp(float temperature) {
  // TODO: monitor overshoot
  Log.trace("Zone %s update current temperature %f", m_name.c_str(), temperature);
  m_last_state_change = millis();
  m_current_temp = temperature;
}


void Zone::set_target_temp(float temperature) {
  Log.trace("Zone %s update target temperature %f", m_name.c_str(), temperature);
  m_target_temp = temperature;
  // TODO: publish message?
}
