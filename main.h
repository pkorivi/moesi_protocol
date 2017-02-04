#ifndef MAIN_H
#define MAIN_H

#include <systemc.h>
#include "Cache.h"
#include "aca2009.h"
#include <iostream>
#include <fstream>
#include "cache_module.h"
#include "cpu_module.h"
#include "bus_module.h"

static const uint dummy_value= 0xffffffff;
static const uint MEM_SIZE = 0x1ff;//0xffffffff;  // 4GB
extern std::ofstream file;

//#define  __DEBUG__

#endif
