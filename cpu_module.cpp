#include "main.h"
#include "cpu_module.h"
void CPU::execute()
{
    wait();
    wait();
    TraceFile::Entry    tr_data;
    CACHE_MODULE::function  f;
    // Loop until end of tracefile

#ifdef __DEBUG__
    char dumpenter;
    while(!tracefile_ptr->eof() && cout << "Please enter 'c' for next line > " && cin >> dumpenter && dumpenter == 'c')
#else
    while(!tracefile_ptr->eof())
#endif
    {
        // Get the next action for the processor in the trace
        if(!tracefile_ptr->next(cpu_id, tr_data))
        {
            break;
        }
        switch(tr_data.type)
        {
            case TraceFile::ENTRY_TYPE_READ:
                f = CACHE_MODULE::func_read;
                break;
            case TraceFile::ENTRY_TYPE_WRITE:
                f = CACHE_MODULE::func_write;
                break;

            case TraceFile::ENTRY_TYPE_NOP:
                break;

            default:
                exit(0);
        }

        if(tr_data.type != TraceFile::ENTRY_TYPE_NOP)
        {
            port_memaddr.write(tr_data.addr);
            port_memfunc.write(f);


            if (f == CACHE_MODULE::func_write)
            {

              sc_int<8> data = rand();

              file << sc_time_stamp() << ": CPU "<< cpu_id <<" sends write, address: "<< tr_data.addr << endl;


                port_memdata.write(data);
                //wait();
              //  port_memdata.write("ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ");

            }
            else
            {
                file << sc_time_stamp() << ": CPU "<< cpu_id <<" sends read, address: "<< tr_data.addr << endl;
            }
            wait(port_memdone.value_changed_event());
            /*if (f == CACHE_MODULE::func_read){
                //cout << sc_time_stamp() << ": CPU reads: " << port_memdata.read() << endl;
            }*/
        }
        else
        {
          //  file << sc_time_stamp() << ": CPU "<< cpu_id <<"executes NOP" << endl;
        }
        // Advance one cycle in simulated time
        wait();
    }

    // Finished the Tracefile, now stop the simulation
    sc_stop();
}
