#ifndef KS_BYTES_H
#define KS_BYTES_H

#include <stdint.h>

/*
 * Read little-endian integers from a byte buffer. The lowest byte comes first.
 * These work on any CPU and with any alignment of p.
 */
uint16_t ks_read_u16le(const uint8_t *p);
uint32_t ks_read_u32le(const uint8_t *p);

#endif
