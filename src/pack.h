#ifndef KS_PACK_H
#define KS_PACK_H

#include <stddef.h>
#include <stdint.h>

/*
 * A pack archive (magic "KPK1") that is already loaded in a memory buffer.
 * See docs/FORMATS.md for the layout. This module only checks the header and the
 * index; it never copies the data and never reads outside data[0 .. size-1].
 */

/* Outcome of ks_pack_open. Zero means success, every other value is one error. */
typedef enum ks_pack_status {
    KS_PACK_OK = 0,    /* the pack was opened */
    KS_PACK_TOO_SMALL, /* the file is smaller than the 16 byte header */
    KS_PACK_BAD_MAGIC, /* the first 4 bytes are not "KPK1" */
    KS_PACK_BAD_INDEX, /* entry_count above 65,535, or the index is not inside the file */
    KS_PACK_BAD_CRC    /* the index bytes do not match index_crc */
} ks_pack_status;

/* An opened pack. It points into the caller's buffer and does not own it, so the
 * buffer must stay alive and unchanged for as long as the ks_pack is used. */
typedef struct ks_pack {
    const uint8_t *data;  /* start of the file image */
    size_t size;          /* length of the file image in bytes */
    uint32_t entry_count; /* number of index rows, at most 65,535 */
    const uint8_t *index; /* first index byte, at index_offset, 16 bytes per row */
} ks_pack;

/*
 * Checks the header and the index of the pack in data[0 .. size-1].
 * Returns KS_PACK_OK and fills in *pack when everything is right. On an error it
 * returns the matching ks_pack_status and the content of *pack is undefined.
 * The checks run in this order: size, magic, entry_count and index bounds, index CRC.
 */
ks_pack_status ks_pack_open(ks_pack *pack, const uint8_t *data, size_t size);

#endif
