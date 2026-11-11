#include "pack.h"

#include "bytes.h"
#include "crc32.h"

#define KS_PACK_HEADER_SIZE 16u
#define KS_PACK_ROW_SIZE 16u
#define KS_PACK_MAX_ENTRIES 65535u

ks_pack_status ks_pack_open(ks_pack *pack, const uint8_t *data, size_t size) {
    if (size < KS_PACK_HEADER_SIZE) {
        return KS_PACK_TOO_SMALL;
    }
    if (data[0] != 'K' || data[1] != 'P' || data[2] != 'K' || data[3] != '1') {
        return KS_PACK_BAD_MAGIC;
    }

    uint32_t entry_count = ks_read_u32le(data + 4);
    uint32_t index_offset = ks_read_u32le(data + 8);
    uint32_t index_crc = ks_read_u32le(data + 12);

    if (entry_count > KS_PACK_MAX_ENTRIES) {
        return KS_PACK_BAD_INDEX;
    }
    /* Never add index_offset and entry_count * 16: both are u32 and the sum can
     * overflow 32 bits. Compare against the bytes that are left in the file. */
    if ((size_t)index_offset > size) {
        return KS_PACK_BAD_INDEX;
    }
    size_t index_bytes = (size_t)entry_count * KS_PACK_ROW_SIZE;
    if (index_bytes > size - (size_t)index_offset) {
        return KS_PACK_BAD_INDEX;
    }

    const uint8_t *idx = data + index_offset;
    if (ks_crc32(0, idx, index_bytes) != index_crc) {
        return KS_PACK_BAD_CRC;
    }

    pack->data = data;
    pack->size = size;
    pack->entry_count = entry_count;
    pack->index = idx;
    return KS_PACK_OK;
}
