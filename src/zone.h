#pragma once

#include <Particle.h>

class Zone
{
  public:
    Zone(String name, int pin);
    ~Zone();

    const char *name() { return m_name; }
    void loop();
    void set_current_temp(float temperature);
    int set_target_temp(float temperature);

    // Particle function:
    int set_target_temp_cloud(String temperature);

  private:
    char m_name[16];
    int m_pin;

    double m_current_temp;
    system_tick_t m_last_current_temp;

    double m_target_temp;

    bool m_heating;
    system_tick_t m_last_state_change;

    void turn_heat(bool on);
};
