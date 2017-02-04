#include "main.h"
#include "bus_module.h"

bool Bus::read(int writer, sc_uint<32> addr){
    /* Try to get exclusive lock on bus. */
    //cout<<"Read::writer "<<writer<<endl;
    while(bus.trylock() == -1){
        /* Wait when bus is in contention. */
        waits++;
        wait();
    }

    /* Update number of bus accesses. */
    reads++;
    write_share_low();
    bus_signal_func = 1;


    /* Set lines. */
    Port_BusAddr.write(addr);
    Port_BusWriter.write(writer);
    Port_Busfunc.write(CACHE_MODULE::func_read);

    /* Wait for everyone to recieve. */
    wait();
    bus_signal_func=0;

    /* Reset. */
  //  Port_Busfunc.write(CACHE_MODULE::func_invalid);
    //Port_BusAddr.write("ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ");
    bus.unlock();

    return(true);
}

/* Write action to memory, need to know the writer, address and data. */
bool Bus::write(int writer, sc_uint<32> addr, int data){
    //cout<<"write::writer "<<writer<<endl;
    /* Try to get exclusive lock on the bus. */
    while(bus.trylock() == -1){
        waits++;
        wait();
    }
    /* Update number of accesses. */
    writes++;
    /* Set. */
    Port_BusAddr.write(addr);
    Port_BusWriter.write(writer);
    Port_Busfunc.write(CACHE_MODULE::func_write);
    bus_signal_func = 2;
    /* Wait for everyone to recieve. */
    wait();
    bus_signal_func = 0;
    /* Reset. */
    //Port_Busfunc.write(CACHE_MODULE::func_invalid);
  //  Port_BusAddr.write("ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ");
    bus.unlock();
    return(true);
}

/* Write action to memory, need to know the writer, address and data. */
bool Bus::write_rdx(int writer, sc_uint<32> addr){
  /* Try to get exclusive lock on bus. */
  //cout<<"Read::writer "<<writer<<endl;
  while(bus.trylock() == -1){
      /* Wait when bus is in contention. */
      waits++;
      wait();
  }

  /* Update number of bus accesses. */
  read_ex++;
  write_share_low();
  bus_signal_func =3;


  /* Set lines. */
  Port_BusAddr.write(addr);
  Port_BusWriter.write(writer);
  Port_Busfunc.write(CACHE_MODULE::func_rd_x);

  /* Wait for everyone to recieve. */
  wait();
  bus_signal_func=0;

  /* Reset. */
  //Port_Busfunc.write(CACHE_MODULE::func_invalid);
  //Port_BusAddr.write("ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ");
  bus.unlock();

  return(true);
}
/* Write action to memory, need to know the writer, address and data. */
bool Bus::write_upgr(int writer, sc_uint<32> addr){
    //cout<<"write::writer "<<writer<<endl;
    /* Try to get exclusive lock on the bus. */
    while(bus.trylock() == -1){
        waits++;
        wait();
    }
    /* Update number of accesses. */
    upgrades++;
    bus_signal_func = 4;
    /* Set. */
    Port_BusAddr.write(addr);
    Port_BusWriter.write(writer);
    Port_Busfunc.write(CACHE_MODULE::func_upgr);
    /* Wait for everyone to recieve. */
    wait();
    bus_signal_func = 0;
    /* Reset. */
  //  Port_Busfunc.write(CACHE_MODULE::func_invalid);
    //Port_BusAddr.write("ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ");
    bus.unlock();
    return(true);
}

bool Bus::write_share_high(){
  Port_Share.write(1);
  shared_signal = 1;
  return(true);
}

bool Bus::write_share_low(){
  Port_Share.write(0);
  shared_signal = 0;
  return(true);
}
/* Bus output. */
void Bus::output(){
    /* Write output as specified in the assignment. */
    double avg = (double)waits / double(reads + writes);
   printf("\n 2. Main memory access rates\n");
    printf("    Bus had %ld reads, %ld reads_ex, %ld writes and %ld upgrades.\n", reads, read_ex, writes, upgrades);
    printf("    A total of %ld accesses.\n", reads + read_ex + writes + upgrades);
    printf("\n 3. Average time for bus acquisition\n");
    printf("    There were %ld waits for the bus.\n", waits);
    printf("    Average waiting time per access: %f cycles.\n", avg);
}
