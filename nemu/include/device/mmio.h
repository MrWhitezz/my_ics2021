#ifndef __DEVICE_MMIO_H__
#define __DEVICE_MMIO_H__

#include <common.h>
#include "map.h" // add to offer fetch_mmio_map

word_t mmio_read(paddr_t addr, int len);
void mmio_write(paddr_t addr, int len, word_t data);
IOMap* fetch_mmio_map(paddr_t addr);// Add for implementation for DTRACE

#endif
