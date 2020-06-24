#include "mbed.h"

class AggregatMotor {

    public:

        // class Controller {
        //     virtual void set(float pos) = 0;
        //     virtual float get() = 0;
        // };

    protected:

        // Controller &m_controller;

        PwmOut m_pwm;

        int m_refresh_rate_hz;
        int m_pulsewidth_usec_min;
        int m_pulsewidth_usec_max;

        int m_pulsewidth_usec_current;

    public:

        AggregatMotor(PinName pwm_pin_name, int refresh_rate_hz, int pulsewidth_usec_min, int pulsewidth_usec_max);
        AggregatMotor(const PinMap &pwm_pin_map, int refresh_rate_hz, int pulsewidth_usec_min, int pulsewidth_usec_max);

        void set_refresh_rate(int refresh_rate_hz);

        void set(float pos);
        float get();

        void suspend();
        void resume();

        AggregatMotor& operator=(float dc){
            set(dc);
            return *this;
        }

        operator float(){
            return get();
        }

        // void run();

    protected:

        void init();

};