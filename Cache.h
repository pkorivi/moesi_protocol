#ifndef cache_H
#define cache_H
#include <systemc.h>
using namespace std;

//static const int MEM_SIZE = 512;

/****************************************
*	----------------------
*	| V |  TAG  |  Data  |
*	----------------------
*****************************************/
static const uint ARCHX = 32; //TODO update 32 byte ~ 8

enum status{
	miss,
	hit
};
/*
enum cache_block_states{
	invalid,
	valid
};*/

enum cache_block_states
{
		INVALID,
		EXCLUSIVE,
		SHARED,
		MODIFIED,
		OWNED,
		NO_COPIES
};
struct cacheLine32B
{
	cache_block_states block_status;
	uint Tag;
	sc_int<8> Data[ARCHX];
	sc_core::sc_time timeStamp;
	sc_uint<8> lru_bits;
	cacheLine32B() : block_status(NO_COPIES), Tag(0xffffffff)
	{
		for(uint i = 0; i < ARCHX; i++)
			Data[i]= 0;
	};
};


class Cache
{
	private:
		uint numberOfSets;
		uint blockSize;
		uint ways;
		uint size;
		uint offset;
		uint setSize;
		cacheLine32B** cacheSim;

	public:
//constructor
		Cache(){};
		Cache(uint S, uint A, uint B);
//destructor
		~Cache();
		void initCache(uint S, uint A, uint B);

		void printCache();
		void printCacheValue(uint address);
		void printCacheLine(uint line);
		int getCacheValue(uint word_offset, uint line, uint way);
		void writeCache(sc_int<8> data, uint tag, uint line, uint way, uint word_offset,
										cache_block_states value = MODIFIED);
		void write_block_mem_to_cache(uint tag, uint line, uint way, cache_block_states value = EXCLUSIVE);

		void setBlockStatus(cache_block_states value, uint line, uint way);
		cache_block_states getBlockStatus(uint line, uint way);
		void setTag(uint value, uint line, uint way);

		uint getWay(uint address);
		uint getLine(uint address);
		uint getTag(uint address);
		uint getWordOffset(uint address);
		uint getLruWay(uint line);
		sc_core::sc_time getTimeStamp(uint line, uint way);

		status isHit(uint line_number,uint addr_tag,uint *hit_way);
};

#endif
