#ifndef bus_module_H
#define bus_module_H
#include <iostream>
#include <systemc.h>
#include "bus_interface.h"
using namespace std;

//#include "main.h"
//#include "cache_module.h"


/* Bus class, provides a way to share one memory in multiple CPU + Caches. */
class Bus : public Bus_if, public sc_module {
public:
    /* Ports andkkk  vb Signals. */
    sc_in<bool>      Port_CLK;
    sc_out<CACHE_MODULE::function> Port_Busfunc;
    sc_out<int>      Port_BusWriter;
    sc_out<sc_uint<32> >      Port_BusAddr;
    sc_out<int>      Port_Share;
    sc_out<int>      Port_Flush;
    sc_signal_rv<32> Port_Upgrade;

    //sc_signal_rv<32> Port_BusState;
    /* Bus mutex. */
    sc_mutex bus;
    /* Variables. */
    long waits;
    long reads;
    long writes;
    long upgrades;
    long read_ex;
    sc_uint<8> shared_signal;
    sc_uint<8> bus_signal_func;
    /* Constructor. */
    SC_CTOR(Bus) {
        /* Handle Port_CLK to simulate delay */
        sensitive << Port_CLK.pos();
        // Initialize some bus properties
        //Port_BusAddr.write("ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ");
        /* Update variables. */
        waits = 0;
        reads = 0;
        writes = 0;
        upgrades = 0;
        read_ex = 0;
    }

    /* Perform a read access to memory addr for CPU #writer. */
    virtual bool read(int writer, sc_uint<32> addr);
    /* Write action to memory, need to know the writer, address and data. */
    virtual bool write(int writer, sc_uint<32> addr, int data);
    virtual bool write_rdx(int writer, sc_uint<32> addr);
    virtual bool write_upgr(int writer, sc_uint<32> addr);
    virtual bool write_share_high();
    virtual bool write_share_low();
    /* Bus output. */
    void output();
  };


#endif
