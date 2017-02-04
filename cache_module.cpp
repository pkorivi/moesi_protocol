#include "main.h"
#include "cache_module.h"

/* Thread that handles the bus. */
void CACHE_MODULE::bus(){
    /* Continue while snooping is activated. */
    while(true){
      /* Wait for work. */
      wait(Port_Busfunc.value_changed_event());
      bus_f = Port_Busfunc.read();
      sc_uint<32> addr = Port_BusAddr.read();
      /* Possibilities. */
      if(Port_BusWriter.read() != cache_id)
      {
        bus_line_number = cache_obj.getLine(addr);
        bus_addr_tag = cache_obj.getTag(addr);
        bus_word_offset = cache_obj.getWordOffset(addr);
        bus_cache_status = cache_obj.isHit(bus_line_number,bus_addr_tag, &bus_hit_way);

        bus_cache_block_status = cache_obj.getBlockStatus(bus_line_number, bus_hit_way);
      file << sc_time_stamp()
             << " CACHE Module::Bus:Cache_id "  << cache_id
             << " Func: " << bus_f
             << " Address: " << addr
             << " Line Number: " << dec << bus_line_number
             << " Status: " << bus_cache_status
             << " State: " << bus_cache_block_status;

        if(bus_cache_status == hit){
          if(bus_f == func_read)
            bus_read_count++;
          else if(bus_f == func_rd_x)
            bus_read_ex_count++;
          else if(bus_f == func_upgr)
            bus_read_upgr_count++;
          switch(bus_cache_block_status){
            case INVALID:
                if((bus_f == func_read) ||
                   (bus_f == func_rd_x) ||
                   (bus_f == func_upgr))
                  {
                    file << " -> 0" << endl;
                    //DO NOTHING
                  }

                  break;
            case EXCLUSIVE:
            case MODIFIED:
                  if(bus_f == func_rd_x)
                    {
                      //Flush values
                      //Invalidate itself
                      cache_obj.setBlockStatus(INVALID, bus_line_number, bus_hit_way);
                      file << " -> " << cache_obj.getBlockStatus(bus_line_number,bus_hit_way) << endl;
                    }
                    else if(bus_f == func_read)
                    {
                      cache_obj.setBlockStatus(OWNED, bus_line_number, bus_hit_way);
                      file << " -> " << cache_obj.getBlockStatus(bus_line_number, bus_hit_way) << endl;
                    }
                    //Port_BusFlush.write(cache_obj.getCacheValue(word_offset, line_number, hit_way));
                    Port_Bus->write_share_high();
                    //Port_BusShare.write(1);
                          break;
            case SHARED:
                    if(bus_f == func_rd_x || bus_f == func_upgr)
                    {
                      cache_obj.setBlockStatus(INVALID, bus_line_number, bus_hit_way);
                      file << " -> " << cache_obj.getBlockStatus(bus_line_number, bus_hit_way) << endl;
                    }
                      break;

            case OWNED:
            if(bus_f== func_rd_x)
              {
                //Flush values
                //Invalidate itself
                cache_obj.setBlockStatus(INVALID, bus_line_number, bus_hit_way);
                file << " -> " << cache_obj.getBlockStatus(bus_line_number, bus_hit_way) << endl;
                //Port_BusFlush.write(cache_obj.getCacheValue(word_offset, line_number, hit_way));
                Port_Bus->write_share_high();
                //Port_BusShare.write(1);
              }
              else if(bus_f == func_read)
              {
                cache_obj.setBlockStatus(OWNED, bus_line_number, bus_hit_way);
                file << " -> " << cache_obj.getBlockStatus(bus_line_number, bus_hit_way) << endl;
                //file << " state E -> O "<< endl;
                //SEND to bus
                //Port_BusFlush.write(cache_obj.getCacheValue(word_offset, line_number, hit_way));
                Port_Bus->write_share_high();
                //Port_BusShare.write(1);
              }
              else if(bus_f== func_upgr)
              {
                cache_obj.setBlockStatus(INVALID, bus_line_number, bus_hit_way);
                file << " -> " << cache_obj.getBlockStatus(bus_line_number, bus_hit_way) << endl;
                //Invalidate itself
              }
                      break;
              default:
                  break;
          }// switch end
        }// is hit
        else
        {
          file << " no block"<< endl;
        }
      }
    }
}

void CACHE_MODULE::execute(){
  while(true){
    wait();
    wait(port_func.value_changed_event());
    f = port_func.read();
    sc_uint<32> addr = port_addr.read();
    //Look for presence in cache
    line_number = cache_obj.getLine(addr);
    addr_tag = cache_obj.getTag(addr);
    word_offset = cache_obj.getWordOffset(addr);
    cache_status = cache_obj.isHit(line_number,addr_tag, &hit_way);
    cache_block_status = cache_obj.getBlockStatus(line_number, hit_way);
    if(cache_status == miss){
      cache_temp = 0;
    }
    else
      cache_temp = 1;

    log("exe", addr);

        // <<endl;

    if(f == func_write){
      data = port_data.read().to_int();
      f_temp = 1;
    }
    else
      f_temp = 0;
    //simulate cache read/write delay

    if (f == func_read)
    {
      //cout << sc_time_stamp() << ": Cache received read (hit/miss =  " << cache_status <<")"<<endl;
      if(cache_status == miss)
      {
          stats_readmiss(cache_id);
          ReadFromMemory(addr);
          file << cache_obj.getBlockStatus(line_number, hit_way)<< endl;
      }
      else
      {
          switch(cache_block_status){
            case INVALID:
  	          stats_readmiss(cache_id);
              InvalidStateRead(addr);
              log("exe:Invalid", addr);
              //file <<" CACHE Module: "  << cache_id << " > " << cache_obj.getBlockStatus(line_number, hit_way)<< endl;
              break;

            case EXCLUSIVE:
            //Do nothing stay in the same state
            case SHARED:
            //Do nothing stay in the same state
            case MODIFIED:
            //Do nothing stay in the same state
            case OWNED:
            //Do nothing stay in the same state
             stats_readhit(cache_id);
                          break;
           default:
                              break;
          }

      }

      int tmp = cache_obj.getCacheValue(word_offset, line_number, hit_way);

      //cout << "return value from read = " << tmp << endl;
      port_data.write(tmp);
      //cout << "ret_read_done =" << ret_read_done <<endl;

      port_done.write(ret_read_done);
      //wait();
      //port_data.write("zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz");
    }
    else //Writing to Cache
    {
      if(cache_status == hit){

        switch(cache_obj.getBlockStatus(line_number,hit_way)){
          case INVALID:
              // Send BusreadX
              // Write to cache
              // change the state to MODIFIED
  	          stats_writemiss(cache_id);
              InvalidStateWrite(addr, data);
              log("exe:Invalid:W", addr);
              //file <<" CACHE Module: "  << cache_id << " > " << cache_obj.getBlockStatus(line_number, hit_way)<< endl;
              break;
          case EXCLUSIVE:
              //  Write to cache
              // change the state to MODIFIED
  	          stats_writehit(cache_id);
              cache_obj.writeCache(data,addr_tag,line_number,hit_way,word_offset, MODIFIED);
              log("exe:EXCLUSIVE:W", addr);
              //file <<" CACHE Module: "  << cache_id << " > " << cache_obj.getBlockStatus(line_number, hit_way)<< endl;
              break;
          case SHARED:
              // BusUpgrade
              // Write To cache
              // change the state to MODIFIED
  	          stats_writehit(cache_id);
              Port_Bus->write_upgr(cache_id,addr);
              cache_obj.writeCache(data,addr_tag,line_number,hit_way,word_offset, MODIFIED);
              log("exe:SHARED:W", addr);
              //file <<" CACHE Module: "  << cache_id << " > " << cache_obj.getBlockStatus(line_number, hit_way)<< endl;
              break;
          case MODIFIED:
              // Write To cache
  	          stats_writehit(cache_id);
              cache_obj.writeCache(data,addr_tag,line_number,hit_way,word_offset, MODIFIED);
              log("exe:MODIFIED:W", addr);
              //file <<" CACHE Module: "  << cache_id << " > " << cache_obj.getBlockStatus(line_number, hit_way)<< endl;
              break;
          case OWNED:
              // BusUpgrade
              // Write To cache
              // change the state to MODIFIED
  	          stats_writehit(cache_id);
              Port_Bus->write_upgr(cache_id,addr);
              cache_obj.writeCache(data,addr_tag,line_number,hit_way,word_offset, MODIFIED);
              log("exe:OWNED:W", addr);
              //file <<" CACHE Module: "  << cache_id << " > " << cache_obj.getBlockStatus(line_number, hit_way)<< endl;
              break;
          default:
              break;
        }

      } //write miss
      else
      {
        stats_writemiss(cache_id);
        WriteExFromMemory(addr, data);
        log("exe:W:Miss", addr);
        //file <<" CACHE Module: "  << cache_id << " > " << cache_obj.getBlockStatus(line_number, hit_way)<< endl;
      }

      port_done.write(ret_write_done);

    }
  }

}

//READ MISS
//the address in not in the cache
//IF there is a valid copy in other cache,
//THEN copy it and set status to SHARED
//ELSE Read From memory, set status to EXCLUSIVE
void CACHE_MODULE::ReadFromMemory(sc_uint<32> addr)
{

    Port_Bus->read(cache_id,addr); //send bus read
  //  wait();

    lru_way = cache_obj.getLruWay(line_number);
    if((cache_obj.getBlockStatus(line_number, lru_way) == OWNED)
      || (cache_obj.getBlockStatus(line_number, lru_way) == MODIFIED))
    {
      Port_Bus->write(cache_id,addr, 4);
    }
    if(Port_BusShare.read() == 1)
	    {
        shared_counter++;
		//assuming that data received from the other cache
          cache_obj.write_block_mem_to_cache(addr_tag,line_number,lru_way, SHARED);
      } else
    	{
    		wait(99);
    		cache_obj.write_block_mem_to_cache(addr_tag,line_number,lru_way, EXCLUSIVE);
    	}

}

//THE address is in the cache in state INVALID
//IF there is a valid copy in other cache,
//THEN copy it and set status to SHARED
//ELSE Read From memory, set status to EXCLUSIVE
void CACHE_MODULE::InvalidStateRead(sc_uint<32> addr)
{
  //Port_BusShare.write(0);
  Port_Bus->read(cache_id,addr); //send bus read
  //wait();

  if(Port_BusShare.read() == 1)
  {
    //temp_shared = 1;
      shared_counter++;
  //  cache_obj.write_block_mem_to_cache(addr_tag,line_number,hit_way, SHARED);
    cache_obj.writeCache(4,addr_tag,line_number,hit_way,word_offset,SHARED);
  }
  else
  {
  	 wait(99);
     //lru_way = cache_obj.getLruWay(line_number);
     //cache_obj.write_block_mem_to_cache(addr_tag,line_number,lru_way, SHARED);
	   cache_obj.writeCache(4,addr_tag,line_number,hit_way,word_offset,EXCLUSIVE);
  }
}


//the address in not in the cache
//IF there is a valid copy in other cache,
//THEN copy it and set status to SHARED
//ELSE Read From memory, set status to EXCLUSIVE
void CACHE_MODULE::WriteExFromMemory(sc_uint<32> addr, int data)
{

    Port_Bus->write_rdx(cache_id,addr); //send bus read
    //wait();
    lru_way = cache_obj.getLruWay(line_number);
    if((cache_obj.getBlockStatus(line_number, lru_way) == OWNED)
      || (cache_obj.getBlockStatus(line_number, lru_way) == MODIFIED))
    {
      Port_Bus->write(cache_id,addr, 4);
    }

    //assuming that data received from the other cache
    cache_obj.write_block_mem_to_cache(addr_tag,line_number,lru_way, MODIFIED);
    if(Port_BusShare.read() == 1)
    {
        shared_counter++;
    }
    else{
      wait(99);
    }

}

//THE address is in the cache in state INVALID
//IF there is a valid copy in other cache,
//THEN copy it and set status to SHARED
//ELSE Read From memory, set status to EXCLUSIVE
void CACHE_MODULE::InvalidStateWrite(sc_uint<32> addr, int data)
{
  //Port_BusShare.write(0);
  Port_Bus->write_rdx(cache_id,addr); //send bus read
  //wait(); // Everyone receives signal

  if(Port_BusShare.read() == 1)
  {
    //temp_shared = 1;
      shared_counter++;
    cache_obj.writeCache(data,addr_tag,line_number,hit_way,word_offset, MODIFIED);
  }
  else
  {
    wait(99);
    lru_way = cache_obj.getLruWay(line_number);
    cache_obj.write_block_mem_to_cache(addr_tag,line_number,lru_way, MODIFIED);
    cache_obj.writeCache(data,addr_tag,line_number,hit_way,word_offset, MODIFIED);
  }
}


void CACHE_MODULE::log(string str, sc_uint<32> addr)
{
  file << sc_time_stamp()
       << " CACHE Module::"   << str << ": "
       << cache_id
       << " Line Number: " << dec << line_number
       << " address tag: " << addr_tag
       << " word_offset: " << word_offset
       << " cache_status: " << cache_status
       << " Address: "  << addr
       << " data: " << data
       << " RW/WR: " << f
       << " State: " <<  cache_obj.getBlockStatus(line_number, hit_way) << endl;
}
void CACHE_MODULE::print_snoop_stats(){
    cout<<"Valid Snoop Stats::"<< cache_id <<
          " read "<<bus_read_count<<
          " read_ex "<<bus_read_ex_count<<
          " upgrade "<<bus_read_upgr_count<<
          " Total:: "<<(bus_read_count+bus_read_ex_count+bus_read_upgr_count)<<
          " Shared counter: " << shared_counter << endl;

}
