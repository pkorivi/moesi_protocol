#include "main.h"
#include <string>
#include <vector>
using namespace std;
std::ofstream file("log.txt");

sc_trace_file *wf = sc_create_vcd_trace_file("wf_cache");
int sc_main(int argc, char* argv[])
{


    file << "Parameter file  "<<argv[1]<<"  " << endl;
    try{
      //sc_report_handler::set_actions (SC_ID_VECTOR_CONTAINS_LOGIC_VALUE_,
      //                          SC_DO_NOTHING);
      sc_report_handler::set_actions(SC_WARNING, SC_DO_NOTHING);

      // Get the tracefile argument and create Tracefile object
      // This function sets tracefile_ptr and num_cpus
      init_tracefile(&argc, &argv);
		const uint numberOfCPUs = tracefile_ptr->get_proc_count();
		file << "Number of CPUs: " << numberOfCPUs << endl;
		cout << "Number of CPUs: " << numberOfCPUs << endl;
      // Initialize statistics counters
      stats_init();
    // Instantiate Modules
  	CACHE_MODULE * cache[numberOfCPUs] ;
  	CPU * cpu[numberOfCPUs];
    /* Create Bus and TraceFile Syncronizer. */
    Bus  bus("bus");
    //signals
    sc_buffer<CACHE_MODULE::function>   sigmemfunc[numberOfCPUs];
    sc_buffer<CACHE_MODULE::retcode>    sigmemdone[numberOfCPUs];
    sc_signal_rv<32>                    sigmemaddr[numberOfCPUs];
    sc_signal_rv<8>                     sigmemdata[numberOfCPUs];
    /* Create clock. */
    sc_clock clk;

    /* Signals Chache/Bus. */
    sc_signal<int,SC_MANY_WRITERS>      sigBusWriter;
    sc_signal<CACHE_MODULE::function, SC_MANY_WRITERS>   sigBusfunc;
    sc_signal<sc_uint<32> >           sigBusPortAddr;
    sc_signal<int, SC_MANY_WRITERS>   sigBusShare;
    sc_signal<int, SC_MANY_WRITERS>   sigBusPortFlush;
      //sc_signal_resolved<int> sigBusWriter;
    //sc_signal_resolved<CACHE_MODULE::function>   sigBusfunc;

    /* General Bus signals. */
    bus.Port_Busfunc(sigBusfunc);
    bus.Port_BusWriter(sigBusWriter);
    bus.Port_BusAddr(sigBusPortAddr);
    //sigBusPortAddr = bus.Port_BusAddr;
    //sigBusShare = bus.Port_Share;
    //sigBusPortFlush = bus.Port_Flush;
    bus.Port_Flush(sigBusPortFlush);
    bus.Port_Share(sigBusShare);

    //bus.Port_BusAddr(sigBusPortAddr);
    /* Connect to Clock. */

    bus.Port_CLK(clk);

	  for(uint i = 0; i < numberOfCPUs; i++)
	  {
		  cache[i] = new CACHE_MODULE(sc_gen_unique_name("cache_"));
		  cpu[i] = new CPU(sc_gen_unique_name("cpu_"));
			cache[i]->setPid(i);
			cpu[i]->setPid(i);
      //TODO define snooping for cache
      //cache[i]->snooping = snooping;

      /* Cache to Bus. */
    //  cache[i]->Port_BusAddr(bus.Port_BusAddr);
      cache[i]->Port_BusAddr(sigBusPortAddr);
      cache[i]->Port_BusWriter(sigBusWriter);
      cache[i]->Port_Busfunc(sigBusfunc);
      cache[i]->Port_BusShare(sigBusShare);
      cache[i]->Port_BusFlush(sigBusPortFlush);
      cache[i]->Port_Bus(bus);

      /* Cache to CPU. */
			cache[i]->port_func(sigmemfunc[i]);
			cache[i]->port_addr(sigmemaddr[i]);
			cache[i]->port_data(sigmemdata[i]);
			cache[i]->port_done(sigmemdone[i]);
      /* CPU to Cache. */
		  cpu[i]->port_memfunc(sigmemfunc[i]);
			cpu[i]->port_memaddr(sigmemaddr[i]);
			cpu[i]->port_memdata(sigmemdata[i]);
			cpu[i]->port_memdone(sigmemdone[i]);



		  cache[i]->port_clk(clk);
			cpu[i]->port_clk(clk);
		}

      //TODO  modify the variables if needed
        sc_trace(wf, clk, "clock");
    for(uint i = 0; i < numberOfCPUs; i++)
	  {
      sc_trace(wf, cache[i]->line_number, sc_gen_unique_name("line_"));
      sc_trace(wf, cache[i]->cache_temp, sc_gen_unique_name("Hit_Miss_"));
      sc_trace(wf, cache[i]->lru_way, sc_gen_unique_name("lru_way_"));
      sc_trace(wf, cache[i]->f_temp, sc_gen_unique_name("Rd_Wr_"));
      sc_trace(wf, cache[i]->port_addr, sc_gen_unique_name("Address_"));
      sc_trace(wf, cache[i]->hit_way, sc_gen_unique_name("hit_way_"));

      //TODO sc_trace(wf, clk, "shared");

    }
      sc_trace(wf, bus.bus_signal_func, "Bus_Signal");
      sc_trace(wf,bus.shared_signal, "Shared_Signal");

      cout<<"running cache simulation, ctrl+c to exit"<<endl;
      //Start simulation;
      sc_start();
      cout<<"------- CACHE STATISTICS! ----- "<<endl;
      stats_print();
      cout<< "Total Run Time:: "<<sc_time_stamp()<<endl;
      bus.output();

      for(uint i = 0; i < numberOfCPUs; i++){
        cache[i]->print_snoop_stats();
      }

  	 for(uint i = 0; i < numberOfCPUs; i++)
  	  {
        file<< "deleting cache ["<<i<<"] ... "<<endl;
          delete cache[i];
        file<< "deleting cpu ["<<i<<"]... "<<endl;
          delete cpu[i];
  	  }
  } //try module
  catch (exception& e){
      cerr << e.what() << endl;
  }
return 0;
}
