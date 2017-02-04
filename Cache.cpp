#include "Cache.h"
#include <iostream>
#include <math.h>

using namespace std;

Cache::Cache(uint S, uint A, uint B)
{
	blockSize = B;
	ways = A;
	size = S;
	numberOfSets = S/(A*B);
	offset = (uint)log2(B);
	setSize = (uint)log2(numberOfSets);
	cout << "Cache Size:     " << size/1024 << " KB" <<endl;
	cout << "Association:    " << ways <<endl;
	cout << "Block size:     " << blockSize << " Bytes" <<endl;
	cout << "Number of sets: " << numberOfSets <<endl;
	cout << "Set size: 	 " << setSize <<endl;
	cout << "Offset: 	 " << offset <<endl;

// allocate memory for simulation of the cache
	cout << "Allocating memory for cache..."<<endl;
	cacheSim = new cacheLine32B*[numberOfSets];
	for(uint i = 0; i < numberOfSets; ++i)
	{
		cacheSim[i] = new cacheLine32B[ways];
	}
}


void Cache::initCache(uint S, uint A, uint B)
{
	blockSize = B;
	ways = A;
	size = S;
	numberOfSets = S/(A*B);
	offset = (uint)log2(B);
	setSize = (uint)log2(numberOfSets);
	cout << "Cache Size:     " << size/1024 << " KB" <<endl;
	cout << "Association:    " << ways <<endl;
	cout << "Block size:     " << blockSize << " Bytes" <<endl;
	cout << "Number of sets: " << numberOfSets <<endl;
	cout << "Set size: 	 " << setSize <<endl;
	cout << "Offset: 	 " << offset <<endl;

// allocate memory for simulation of the cache
	cout << "Allocating memory for cache..."<<endl;
	cacheSim = new cacheLine32B*[numberOfSets];
	for(uint i = 0; i < numberOfSets; ++i)
	{
		cacheSim[i] = new cacheLine32B[ways];
	}
}
Cache::~Cache()
{
	//file << "Deleting allocated memory for the cache" <<endl;

	for(uint i = 0; i < numberOfSets; ++i)
	{
		delete[] cacheSim[i];
	}
	delete[] cacheSim;
}


void Cache::printCache()
{

	for(uint i = 0; i < numberOfSets; ++i)
	{
		cout<<"Line: "<<i<<": "<<endl;
		for(uint j = 0; j < ways; ++j)
		{
			cout <<" way  "<< j<<" status   "<< cacheSim[i][j].block_status << " tag " << cacheSim[i][j].Tag << " data  ";
			for(uint k = 0; k < ARCHX; k++)
			     cout << cacheSim[i][j].Data[k] <<" ";

			cout<< "  TS  " << cacheSim[i][j].timeStamp  << endl ;
		}
		cout<<endl;
	}

}

void Cache::printCacheValue(unsigned int address)
{
	cout << "Printing Cache Line... "<<endl;
	cout << "      V  |  TAG  | DATA"<<endl;
	for(uint i = 0; i < numberOfSets; ++i)
	{
		cout<<"Line"<<i<<": ";
		for(uint j = 0; j < ways; ++j)
		{
			cout <<"status  "<< cacheSim[i][j].block_status << " tag " << cacheSim[i][j].Tag << " data  ";
			for(uint k = 0; k < ARCHX; k++)
			     cout << cacheSim[i][j].Data[k];

			cout << "; ";
		}
		cout<<endl;
	}

}

void Cache::printCacheLine(uint line)
{
	cout<<"Line: "<<line<<": ";
		for(uint j = 0; j < ways; ++j)
		{
			cout << "Block status: " << cacheSim[line][j].block_status << " Tag: " << cacheSim[line][j].Tag << ", TS:: "<<cacheSim[line][j].timeStamp <<"s  ,";
			/*for(uint k = 0; k < ARCHX; k++)
			     cout << cacheSim[line][j].Data[k]<<" ";*/

			cout << endl;
		}
		cout<<endl;

}

int Cache::getCacheValue(uint word_offset, uint line, uint way){
	cacheSim[line][way].timeStamp = sc_time_stamp();
	return (int)cacheSim[line][way].Data[word_offset];
}

void Cache::writeCache(sc_int<8> data, uint tag, uint line, uint way, uint word_offset, cache_block_states value){
		cacheSim[line][way].Data[word_offset] = data;
		cacheSim[line][way].Tag = tag;
		cacheSim[line][way].block_status = value;
		cacheSim[line][way].timeStamp = sc_time_stamp();
}


/************************************
* checks if the given tag is the same with the one in the line and way
************************************/
status Cache::isHit(uint line_number,uint addr_tag, uint *hit_way){
	for(uint i = 0; i < ways; ++i){
		//if((cacheSim[line_number][i].block_status != INVALID)
		//		&& (cacheSim[line_number][i].Tag == addr_tag)){
		if((cacheSim[line_number][i].block_status != NO_COPIES)
				&& (cacheSim[line_number][i].Tag == addr_tag)){
					*hit_way = i;
			 		return hit;
		}
	}
	return miss;
}

/************************************
* sets Cache Block Status
************************************/
void Cache::setBlockStatus(cache_block_states value, uint line, uint way){
	cacheSim[line][way].block_status = value;
}

cache_block_states Cache::getBlockStatus(uint line, uint way){
	return cacheSim[line][way].block_status;
}

/************************************
* sets tag in the given line and way
************************************/
void Cache::setTag(uint value, uint line, uint way){
	cacheSim[line][way].Tag = value;
}

/************************************
* gets tag by the given address
************************************/
uint Cache::getTag(uint address)
{
	return (address >> (offset+setSize));
}

/************************************
* gets Line by the given address
************************************/
uint Cache::getLine(uint address)
{
	return ((address>>offset)&(numberOfSets-1));
}
/************************************
* gets Line by the given address
************************************/

uint Cache::getWordOffset(uint address){

	return ((address)&(ARCHX-1));
}

/************************************
* gets time stamp  by the given line and way number
************************************/
sc_core::sc_time Cache::getTimeStamp(uint line, uint way)
{
	return cacheSim[line][way].timeStamp;
}

void Cache::write_block_mem_to_cache(uint tag, uint line, uint way, cache_block_states value){
	for(uint i = 0; i<ARCHX; i++)
		writeCache(4,tag,line,way,i, value); // simulating writing  from main memory to cache amd old block to main memory
}


uint Cache::getLruWay(uint line){
	uint lru =0;
	for (uint i = 1; i < ways; i++) {
			if(cacheSim[line][i].timeStamp < cacheSim[line][i-1].timeStamp)
				lru = i;
	}
	return lru;
}
