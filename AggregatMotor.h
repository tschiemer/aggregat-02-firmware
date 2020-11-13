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

#include "mbed.h"

class AggregatMotor {

    public:

        AggregatMotor(PinName pwr_pin_name, PinName pwm_pin_name, int refresh_rate_hz, int pulsewidth_usec_min, int pulsewidth_usec_max);
        // AggregatMotor(const PinMap &pwm_pin_map, int refresh_rate_hz, int pulsewidth_usec_min, int pulsewidth_usec_max);

        void set_refresh_rate(int refresh_rate_hz);

        float get_pos();
        void set_pos(float pos);

        bool get_state(){
            return m_pwr;
        }
        void set_state(bool on_or_off){
            m_pwr = on_or_off;
        }

        void suspend();
        void resume();
        void run();

        operator float(){
            return get_pos();
        }

        AggregatMotor& operator=(float dc){
            set_pos(dc);
            return *this;
        }

        operator double(){
            return get_pos();
        }

        AggregatMotor& operator=(double dc){
            set_pos(dc);
            return *this;
        }
        

        operator bool(){
            return get_state();
        }

        AggregatMotor& operator=(bool on_or_off){
            set_state(on_or_off);
            return *this;
        }

    protected:

        void init();

        // power (relay) control
        DigitalOut m_pwr;

        // servo pwm...
        PwmOut m_pwm;

        int m_refresh_rate_hz;
        int m_pulsewidth_usec_min;
        int m_pulsewidth_usec_max;

        int m_pulsewidth_usec_current;

        bool m_value_changed;

};