#ifndef KS_CRC32_H
#define KS_CRC32_H

#include <stddef.h>
#include <stdint.h>

/*
 * CRC-32 (IEEE 802.3, reflected, polynomial 0xEDB88320), the same checksum zlib and PNG use.
 * Pass 0 as crc for the first call; pass the previous result to continue over more data.
 */
uint32_t ks_crc32(uint32_t crc, const void *data, size_t len);

/* CRC-32 of a zero-terminated string, not counting the terminator. */
uint32_t ks_crc32_str(const char *text);

#endif
