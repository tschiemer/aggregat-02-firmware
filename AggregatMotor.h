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

        int m_period_usec;
        float m_dc_min;
        float m_dc_max;

    public:

        AggregatMotor(PinName pwm_pin_name, int period_usec, float dc_min, float dc_max);
        AggregatMotor(const PinMap &pwm_pin_map, int period_usec, float dc_min, float dc_max);

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

        void runloop();

    protected:

        void init();

};