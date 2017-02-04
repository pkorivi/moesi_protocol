#ifndef cache_module_H
#define cache_module_H
#include <string>
#include <iostream>
#include <systemc.h>
#include "Cache.h"
#include "bus_interface.h"

#define READ 1
#define WRITE_RDX 2
using namespace std;
//#include "main.h"
//#include "bus_module.h"

using namespace std;
SC_MODULE(CACHE_MODULE){
public:
  enum function{
    func_invalid,
    func_read,
    func_write,
    func_rd_x,
    func_upgr,
    func_flush
  };
  long bus_read_count;
  long bus_read_ex_count;
  long bus_read_upgr_count;
  enum retcode{
    ret_read_done,
    ret_write_done
  };
  /* Possible line states depending on the cache coherence protocol. */
/*  enum Line_State
  {
      INVALID,
      VALID
  };
*/
  /* Possible line states depending on the cache coherence protocol. */
  /*enum Line_State
  {
      INVALID,
      EXCLUSIVE,
      SHARED,
      MODIFIED,
      OWNED
  };*/
cache_block_states cache_block_status;
cache_block_states bus_cache_block_status;
  //sc_uint<WAY_SIZE> way[SET_PER_WAY]; //TODO what is this variable?
  sc_in<bool>     port_clk;
  sc_in<function> port_func;
  sc_in_rv<32>    port_addr;
  sc_out<retcode> port_done;
  sc_inout_rv<8>  port_data;

  //TODO
  /* Bus snooping ports. */
  sc_in<sc_uint<32> >    Port_BusAddr;
  sc_in<int>      Port_BusWriter;
  sc_in<function> Port_Busfunc;
  sc_in<int>      Port_BusShare;
  sc_in<int>      Port_BusFlush;

  /* Bus requests ports. */
  sc_port<Bus_if> Port_Bus;

  /* Variables. */
  int cache_id;
/*Execution func*/
  Cache cache_obj;
  uint hit_way;
  uint line_number;
  uint lru_way;
  uint addr_tag;
  uint word_offset;
  sc_int<8> data;
  status cache_status;
  function f;
  sc_int<8> cache_temp;
  sc_int<8> f_temp;

  /*BUS func*/
    uint bus_hit_way;
    uint bus_line_number;
    uint bus_lru_way;
    uint bus_addr_tag;
    uint bus_word_offset;
    sc_int<8> bus_data;
    status bus_cache_status;
    function bus_f;
    sc_int<8> bus_cache_temp;
    sc_int<8> bus_f_temp;


    long shared_counter;
  SC_CTOR(CACHE_MODULE)
  {
    bus_read_count = 0;
    bus_read_ex_count = 0;
    bus_read_upgr_count = 0;
	shared_counter = 0;
    SC_THREAD(bus);//TODO
    SC_THREAD(execute);
    sensitive<<port_clk.pos();
    dont_initialize();
    cache_obj.initCache(32768,8,ARCHX); //TODO check if tis ok
  }

  void setPid(uint pid){
    cache_id = pid;
    cout << "Cache " << cache_id <<" is ready!"<< endl;
    };
    void print_snoop_stats();

  ~CACHE_MODULE(){};

  private:
    /* Thread that handles the bus. */
    void bus();
    void execute();
    void InvalidStateRead(sc_uint<32> addr);
    void ReadFromMemory(sc_uint<32> addr);
    void WriteExFromMemory(sc_uint<32> addr, int data);
    void InvalidStateWrite(sc_uint<32> addr, int data);
    void log(string str, sc_uint<32> addr);



};// end of cache module


#endif
