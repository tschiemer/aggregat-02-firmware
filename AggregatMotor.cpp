#include "AggregatMotor.h"

AggregatMotor::AggregatMotor(PinName pwm_pin_name, int period_usec, float dc_min, float dc_max)
    : m_pwm(pwm_pin_name)
{
    m_period_usec = period_usec;
    m_dc_min = dc_min;
    m_dc_max = dc_max;  

    init(); 
}

AggregatMotor::AggregatMotor(const PinMap &pwm_pin_map, int period_usec, float dc_min, float dc_max)
    : m_pwm(pwm_pin_map)
{
    m_period_usec = period_usec;
    m_dc_min = dc_min;
    m_dc_max = dc_max;

    init();
}

void AggregatMotor::init()
{
    m_pwm.period_us(m_period_usec);

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

    float dc = pos * (m_dc_max - m_dc_min) + m_dc_min;

    printf("dc %f", dc);

    m_pwm = dc;
}

float AggregatMotor::get()
{
    float dc = m_pwm;

    return (dc - m_dc_min) / (m_dc_max - m_dc_min);
}

void AggregatMotor::suspend()
{
    m_pwm.suspend();
}

void AggregatMotor::resume()
{
    m_pwm.resume();
}

void AggregatMotor::runloop()
{

}