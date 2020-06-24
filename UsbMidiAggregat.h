#include "USBMIDI.h"

class UsbMidiAggregat : public USBMIDI {

    public:

        UsbMidiAggregat(bool connect_blocking=true, uint16_t vendor_id=0x0700, uint16_t product_id=0x0101, uint16_t product_release=0x0001);
        UsbMidiAggregat(USBPhy *phy, uint16_t vendor_id, uint16_t product_id, uint16_t product_release);

    protected:

        const uint8_t *string_iproduct_desc();

        const uint8_t *string_iinterface_desc();

};
