#pragma once

#include <Particle.h>

class Zone
{
  public:
    Zone(String name, int pin, void (*block_updates)(bool));
    ~Zone();

    const char *name() { return m_name; }
    boolean is_heating() const { return m_heating; }
    double target_temp() const { return m_target_temp; }

    void loop();
    void set_current_temp(float temperature);
    int set_target_temp(float temperature);

    // Particle function:
    int set_target_temp_cloud(String temperature);

  private:
    char m_name[16];
    int m_pin;
    void (*m_block_updates)(bool);

    double m_current_temp;
    system_tick_t m_last_current_temp;

    double m_target_temp;

    bool m_heating;
    system_tick_t m_last_state_change;

    void turn_heat(bool on);
};
