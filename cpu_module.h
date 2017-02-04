#ifndef cpu_module_H
#define cpu_module_H
#include <iostream>
#include <systemc.h>
using namespace std;
//#include "main.h"
//#include "cache_module.h"

SC_MODULE(CPU){
public:
  sc_in<bool>                      port_clk;
  sc_in<CACHE_MODULE::retcode>     port_memdone;
  sc_out<CACHE_MODULE::function>   port_memfunc;
  sc_out_rv<32>                     port_memaddr;
  sc_inout_rv<8>                   port_memdata;
  uint cpu_id;

  //SC_HAS_PROCESS(CPU);
  SC_CTOR(CPU)
  //CPU(): sc_module(sc_gen_unique_name("CPU1")), myID(0)
  {
      SC_THREAD(execute);
      sensitive << port_clk.pos();
      dont_initialize();
  }

  void setPid(uint pid){
    cpu_id = pid;
    cout << "CPU " << cpu_id <<" is ready!"<< endl;
    };

private:
  void execute();
};

#endif
