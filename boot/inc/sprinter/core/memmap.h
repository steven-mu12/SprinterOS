#ifndef __MEMMAP_H__
#define __MEMMAP_H__

#include <stdint.h>

extern uint8_t _os_load_addr[];
extern uint8_t _os_load_end[];
extern uint8_t _userspace_start[];
extern uint8_t _userspace_end[];

#define OS_LOAD_ADDR    ((uint32_t)_os_load_addr)
#define OS_LOAD_SIZE    ((uint32_t)(_os_load_end - _os_load_addr))
#define OS_MAX_BLOCKS   (OS_LOAD_SIZE / 512)

#endif
