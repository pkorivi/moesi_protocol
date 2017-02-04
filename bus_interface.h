#ifndef bus_interface_H
#define bus_interface_H

class Bus_if : public virtual sc_interface
{
    public:
        virtual bool read(int writer, sc_uint<32> addr) = 0;
        virtual bool write(int writer, sc_uint<32> addr, int data) = 0;
        virtual bool write_rdx(int writer, sc_uint<32> addr) = 0;
        virtual bool write_upgr(int writer, sc_uint<32> addr) = 0;
        virtual bool write_share_high() = 0;
        virtual bool write_share_low() = 0;
};

#endif
