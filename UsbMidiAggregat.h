#ifndef USBMIDI_AGGREGAT_H
#define USBMIDI_AGGREGAT_H

#include "USBMIDI.h"
#include "PinNames.h"
#include "InterruptIn.h"

class UsbMidiAggregat : public USBMIDI {

    public:

        UsbMidiAggregat(PinName power_pin, bool connect_blocking=true, uint16_t vendor_id=0x0700, uint16_t product_id=0x0101, uint16_t product_release=0x0001);
        UsbMidiAggregat(PinName power_pin, USBPhy *phy, uint16_t vendor_id, uint16_t product_id, uint16_t product_release);

        bool connected();

        bool just_reconnected();

    protected:

        void init_vbus();

        mbed::InterruptIn vbus;

        bool vbus_flag;

        const uint8_t *string_iproduct_desc();

        const uint8_t *string_iinterface_desc();

};

#endif //USBMIDI_AGGREGAT_H