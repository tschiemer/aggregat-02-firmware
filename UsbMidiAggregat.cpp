#include "UsbMidiAggregat.h"

static UsbMidiAggregat * usbDevice = NULL;

UsbMidiAggregat::UsbMidiAggregat(PinName power_pin, bool connect_blocking, uint16_t vendor_id, uint16_t product_id, uint16_t product_release)
    : vbus(power_pin), USBMIDI(connect_blocking, vendor_id, product_id, product_release)
{
    init_vbus();
}

UsbMidiAggregat::UsbMidiAggregat(PinName power_pin, USBPhy *phy, uint16_t vendor_id, uint16_t product_id, uint16_t product_release)
    : vbus(power_pin), USBMIDI(phy, vendor_id, product_id, product_release)
{
    init_vbus();
}

bool UsbMidiAggregat::connected()
{
    return vbus.read() && configured();
}

bool UsbMidiAggregat::just_reconnected()
{

    bool flag = vbus_flag;
    vbus_flag = false;
    if (flag){
        printf("vbus 1 configured %d\n", configured());
    }
    return flag;
}

void UsbMidiAggregat::init_vbus()
{
    UsbMidiAggregat * self = this;

    vbus.rise([self](){
        self->vbus_flag = true;
    });

    vbus_flag = !!vbus.read();
}

const uint8_t *UsbMidiAggregat::string_iinterface_desc()
{
    static const uint8_t string_iinterface_descriptor[] = {
        0x0c,                           //bLength
        STRING_DESCRIPTOR,              //bDescriptorType 0x03
        'A', 0, 'u', 0, 'd', 0, 'i', 0, 'o', 0 //bString iInterface - Audio
    };
    return string_iinterface_descriptor;
}

const uint8_t *UsbMidiAggregat::string_iproduct_desc()
{
    static const uint8_t string_iproduct_descriptor[] = {
        24,                                                       //bLength
        STRING_DESCRIPTOR,                                          //bDescriptorType 0x03
        'A', 0, 'g', 0, 'g', 0, 'r', 0, 'e', 0, 'g', 0, 'a', 0, 't', 0, '-', 0, '0', 0, '2', 0 //bString iProduct - Aggregat-02
    };
    return string_iproduct_descriptor;
}