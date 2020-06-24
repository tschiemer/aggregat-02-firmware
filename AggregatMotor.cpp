#include "AggregatMotor.h"

AggregatMotor::AggregatMotor(PinName pwm_pin_name, int refresh_rate_hz, int pulsewidth_usec_min, int pulsewidth_usec_max)
    : m_pwm(pwm_pin_name)
{
    set_refresh_rate(refresh_rate_hz);
    
    m_pulsewidth_usec_min = pulsewidth_usec_min;
    m_pulsewidth_usec_max = pulsewidth_usec_max;  

    init(); 
}

AggregatMotor::AggregatMotor(const PinMap &pwm_pin_map, int refresh_rate_hz, int pulsewidth_usec_min, int pulsewidth_usec_max)
    : m_pwm(pwm_pin_map)
{
    set_refresh_rate(refresh_rate_hz);

    m_pulsewidth_usec_min = pulsewidth_usec_min;
    m_pulsewidth_usec_max = pulsewidth_usec_max;  

    init();
}

void AggregatMotor::set_refresh_rate(int refresh_rate_hz)
{
    m_refresh_rate_hz = refresh_rate_hz;

    float period_sec = 1.0 / ((float)refresh_rate_hz);

    m_pwm.period(period_sec);
}

void AggregatMotor::init()
{
    // start suspended
    m_pwm.suspend();
}


void AggregatMotor::set(float pos)
{
    // Guard
    if (pos < 0.0) {
        printf("pos out of range: %f\n", pos);
        pos = 0.0;
    } else if (pos > 1.0) {
        printf("pos out of range: %f\n", pos);
        pos = 1.0;
    }

    m_pulsewidth_usec_current = pos * (m_pulsewidth_usec_max - m_pulsewidth_usec_min) + m_pulsewidth_usec_min;

    // printf("pw %d", pw);

    m_pwm.pulsewidth_us(m_pulsewidth_usec_current);
}

float AggregatMotor::get()
{
    return (float)(m_pulsewidth_usec_current - m_pulsewidth_usec_min) / (float)(m_pulsewidth_usec_max - m_pulsewidth_usec_min);
}

void AggregatMotor::suspend()
{
    m_pwm.suspend();
}

void AggregatMotor::resume()
{
    m_pwm.resume();
}

// void AggregatMotor::run()
// {

// }