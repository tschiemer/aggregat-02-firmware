/**
  * aggregat controller 02: MIDI based servo controller
  * Copyright (C) 2020  Institute for Computer Music and Sound Technology
  * (ICST), ZHdK Zurich, <https://www.zhdk.ch/forschung/icst>
  * 
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 3 of the License, or (at your option) any later version.
  * 
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Lesser General Public License for more details.
  * 
  * You should have received a copy of the GNU Lesser General Public License
  * along with this program; if not, write to the Free Software Foundation,
  * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
  */
  
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