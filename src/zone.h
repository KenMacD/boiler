#pragma once

#include <Particle.h>

class Zone
{
  public:
    Zone(String name, int pin);
    ~Zone();

    const String name() { return m_name; }
    void loop();
    void set_current_temp(float temperature);
    void set_target_temp(float temperature);

  private:
    String m_name;
    int m_pin;

    float m_current_temp;
    system_tick_t m_last_current_temp;

    float m_target_temp;

    bool m_heating;
    system_tick_t m_last_state_change;

    void turn_heat(bool on);
};
