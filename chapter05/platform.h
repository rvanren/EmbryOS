#pragma once

// Platform-dependent constants go here

#define PAGE_SHIFT      14
#define PAGE_SIZE       (1 << PAGE_SHIFT)

#ifndef DELAY_MS
#define DELAY_MS        326087	// 1 ms of delay (see syslib.h)
#endif
