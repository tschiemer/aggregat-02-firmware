#include "AggregatMotor.h"
// #include "config.h"

AggregatMotor::AggregatMotor(PinName pwr_pin_name, PinName pwm_pin_name, int refresh_rate_hz, int pulsewidth_usec_min, int pulsewidth_usec_max)
    : m_pwr(pwr_pin_name), m_pwm(pwm_pin_name)
{
    set_refresh_rate(refresh_rate_hz);
    
    m_pulsewidth_usec_min = pulsewidth_usec_min;
    m_pulsewidth_usec_max = pulsewidth_usec_max;  

    m_value_changed = false;

    init(); 
}

void AggregatMotor::init()
{
    // start suspended
    m_pwm.suspend();
}

void AggregatMotor::set_refresh_rate(int refresh_rate_hz)
{
    m_refresh_rate_hz = refresh_rate_hz;

    float period_sec = 1.0 / ((float)refresh_rate_hz);

    m_pwm.period(period_sec);
}

float AggregatMotor::get_pos()
{
    return (float)(m_pulsewidth_usec_current - m_pulsewidth_usec_min) / (float)(m_pulsewidth_usec_max - m_pulsewidth_usec_min);
}

void AggregatMotor::set_pos(float pos)
{
    // Guard
    if (pos < 0.0) {
        printf("pos out of range: %d\n", (int)(100*pos));
        pos = 0.0;
    } else if (pos > 1.0) {
        printf("pos out of range: %d\n", (int)(100*pos));
        pos = 1.0;
    }

    int newval = pos * (m_pulsewidth_usec_max - m_pulsewidth_usec_min) + m_pulsewidth_usec_min;

    // don't update if previous value was identical
    if (newval == m_pulsewidth_usec_current){
        return;
    }

    m_pulsewidth_usec_current = newval;
    m_value_changed = true;

    // printf("usec_pos %d\n", m_pulsewidth_usec_current);

    m_pwm.pulsewidth_us(m_pulsewidth_usec_current);
}


void AggregatMotor::suspend()
{
    m_pwm.suspend();
}

void AggregatMotor::resume()
{
    m_pwm.resume();

    // only update if actually set.
    if (m_value_changed){
        m_value_changed = false;
        m_pwm.pulsewidth_us(m_pulsewidth_usec_current);
    }
}

void AggregatMotor::run()
{
    //
}