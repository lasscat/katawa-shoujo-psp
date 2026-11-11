#include "pack.h"

#include "bytes.h"
#include "crc32.h"
#include "ks_test.h"

#include <string.h>

/*
 * Test packs are built in memory. The valid one has two entries:
 *   bytes 0..15   header: "KPK1", entry_count 2, index_offset 28, index_crc
 *   bytes 16..23  entry 1 data "alpha" (5 bytes) plus 3 zero padding bytes
 *   bytes 24..27  entry 2 data "beta" (4 bytes)
 *   bytes 28..59  index: two rows, name_hash 1 and 2, sorted ascending
 */
#define HEADER_SIZE 16u
#define ROW_SIZE 16u
#define INDEX_AT 28u
#define PACK_SIZE 60u
#define ENTRY1_AT 16u
#define ENTRY1_LEN 5u
#define ENTRY2_AT 24u
#define ENTRY2_LEN 4u
#define MAX_ENTRIES 65535u

static void put_u32le(uint8_t *p, uint32_t v) {
    p[0] = (uint8_t)(v & 0xFFu);
    p[1] = (uint8_t)((v >> 8) & 0xFFu);
    p[2] = (uint8_t)((v >> 16) & 0xFFu);
    p[3] = (uint8_t)((v >> 24) & 0xFFu);
}

/* Builds the valid two-entry pack described above into buf (PACK_SIZE bytes) and returns PACK_SIZE.
 */
static size_t build_pack(uint8_t *buf) {
    static const char entry1[ENTRY1_LEN + 1] = "alpha";
    static const char entry2[ENTRY2_LEN + 1] = "beta";

    memset(buf, 0, PACK_SIZE);
    memcpy(buf, "KPK1", 4);
    put_u32le(buf + 4, 2);
    put_u32le(buf + 8, INDEX_AT);
    put_u32le(buf + 12, 0);
    memcpy(buf + ENTRY1_AT, entry1, ENTRY1_LEN);
    memcpy(buf + ENTRY2_AT, entry2, ENTRY2_LEN);

    put_u32le(buf + INDEX_AT + 0, 1);
    put_u32le(buf + INDEX_AT + 4, ENTRY1_AT);
    put_u32le(buf + INDEX_AT + 8, ENTRY1_LEN);
    put_u32le(buf + INDEX_AT + 12, ks_crc32(0, entry1, ENTRY1_LEN));
    put_u32le(buf + INDEX_AT + ROW_SIZE + 0, 2);
    put_u32le(buf + INDEX_AT + ROW_SIZE + 4, ENTRY2_AT);
    put_u32le(buf + INDEX_AT + ROW_SIZE + 8, ENTRY2_LEN);
    put_u32le(buf + INDEX_AT + ROW_SIZE + 12, ks_crc32(0, entry2, ENTRY2_LEN));

    put_u32le(buf + 12, ks_crc32(0, buf + INDEX_AT, 2 * ROW_SIZE));
    return PACK_SIZE;
}

/*
 * Builds a pack with "count" index rows behind the header, each row all zero except an
 * ascending name_hash, and a matching index_crc. The only problem with the file, if any,
 * is the entry_count itself: the index fits and its CRC is right. Returns the file length.
 */
static size_t build_rows(uint8_t *buf, uint32_t count) {
    size_t index_bytes = (size_t)count * ROW_SIZE;
    memset(buf, 0, HEADER_SIZE + index_bytes);
    memcpy(buf, "KPK1", 4);
    put_u32le(buf + 4, count);
    put_u32le(buf + 8, HEADER_SIZE);
    for (uint32_t i = 0; i < count; i++) {
        put_u32le(buf + HEADER_SIZE + (size_t)i * ROW_SIZE, i);
    }
    put_u32le(buf + 12, ks_crc32(0, buf + HEADER_SIZE, index_bytes));
    return HEADER_SIZE + index_bytes;
}

/* Checks the four u32 fields of one index row. */
static void check_row(const uint8_t *row, uint32_t hash, uint32_t offset, uint32_t len,
                      uint32_t crc) {
    CHECK_EQ_INT(ks_read_u32le(row), hash);
    CHECK_EQ_INT(ks_read_u32le(row + 4), offset);
    CHECK_EQ_INT(ks_read_u32le(row + 8), len);
    CHECK_EQ_INT(ks_read_u32le(row + 12), crc);
}

int main(void) {
    uint8_t buf[PACK_SIZE];
    size_t size;
    ks_pack pack;

    /* ---- a valid pack with two entries ---- */
    size = build_pack(buf);
    CHECK_EQ_INT(size, PACK_SIZE);
    CHECK_EQ_INT(ks_pack_open(&pack, buf, size), KS_PACK_OK);
    CHECK(pack.data == buf);
    CHECK_EQ_INT(pack.size, PACK_SIZE);
    CHECK_EQ_INT(pack.entry_count, 2);
    CHECK(pack.index == buf + INDEX_AT);
    CHECK(memcmp(pack.index, buf + INDEX_AT, 2 * ROW_SIZE) == 0);
    check_row(pack.index, 1, ENTRY1_AT, ENTRY1_LEN, ks_crc32(0, "alpha", ENTRY1_LEN));
    check_row(pack.index + ROW_SIZE, 2, ENTRY2_AT, ENTRY2_LEN, ks_crc32(0, "beta", ENTRY2_LEN));
    CHECK_EQ_INT(pack.data[ENTRY1_AT], 'a');     /* "alpha"[0] */
    CHECK_EQ_INT(pack.data[ENTRY1_AT + 4], 'a'); /* "alpha"[4] */
    CHECK_EQ_INT(pack.data[ENTRY1_AT + 5], 0);   /* zero padding */
    CHECK_EQ_INT(pack.data[ENTRY2_AT], 'b');     /* "beta"[0] */
    CHECK_EQ_INT(pack.data[ENTRY2_AT + 3], 'a'); /* "beta"[3] */

    /* ---- a valid pack with zero entries: header only ---- */
    uint8_t empty[HEADER_SIZE];
    memset(empty, 0, sizeof(empty));
    memcpy(empty, "KPK1", 4);
    put_u32le(empty + 4, 0);
    put_u32le(empty + 8, HEADER_SIZE);
    put_u32le(empty + 12, 0);
    memset(&pack, 0xAA, sizeof(pack));
    CHECK_EQ_INT(ks_pack_open(&pack, empty, sizeof(empty)), KS_PACK_OK);
    CHECK(pack.data == empty);
    CHECK_EQ_INT(pack.size, HEADER_SIZE);
    CHECK_EQ_INT(pack.entry_count, 0);
    CHECK(pack.index == empty + HEADER_SIZE);

    /* ---- the file is smaller than a header ---- */
    size = build_pack(buf);
    CHECK_EQ_INT(ks_pack_open(&pack, buf, HEADER_SIZE - 1u), KS_PACK_TOO_SMALL);
    CHECK_EQ_INT(ks_pack_open(&pack, buf, 0), KS_PACK_TOO_SMALL);
    /* The size check comes first: 3 bytes with a bad magic report the size. */
    uint8_t tiny[4] = {'X', 'Y', 'Z', '1'};
    CHECK_EQ_INT(ks_pack_open(&pack, tiny, 3), KS_PACK_TOO_SMALL);

    /* ---- the magic is wrong ---- */
    size = build_pack(buf);
    buf[3] = '2';
    CHECK_EQ_INT(ks_pack_open(&pack, buf, size), KS_PACK_BAD_MAGIC);
    /* The magic check comes before the index checks. */
    size = build_pack(buf);
    buf[0] = 'k';
    put_u32le(buf + 4, 70000);
    put_u32le(buf + 8, 0xFFFFFFF0u);
    CHECK_EQ_INT(ks_pack_open(&pack, buf, size), KS_PACK_BAD_MAGIC);

    /* ---- entry_count above 65,535, with an index that fits and has the right CRC,
     * so the entry_count alone is the reason to reject the file ---- */
    static uint8_t big[HEADER_SIZE + MAX_ENTRIES * ROW_SIZE + ROW_SIZE];
    size = build_rows(big, 65536);
    CHECK_EQ_INT(size, HEADER_SIZE + 65536u * ROW_SIZE);
    CHECK_EQ_INT(ks_pack_open(&pack, big, size), KS_PACK_BAD_INDEX);
    /* The largest allowed count opens, and every row is inside the file. */
    size = build_rows(big, MAX_ENTRIES);
    CHECK_EQ_INT(size, HEADER_SIZE + MAX_ENTRIES * ROW_SIZE);
    CHECK_EQ_INT(ks_pack_open(&pack, big, size), KS_PACK_OK);
    CHECK_EQ_INT(pack.entry_count, MAX_ENTRIES);
    CHECK(pack.index == big + HEADER_SIZE);
    CHECK_EQ_INT(ks_read_u32le(pack.index), 0);
    CHECK_EQ_INT(ks_read_u32le(pack.index + (MAX_ENTRIES - 1u) * ROW_SIZE), MAX_ENTRIES - 1u);
    /* Counts that are far too big, also in a small file. */
    size = build_pack(buf);
    put_u32le(buf + 4, 65536); /* the index below stays as it was for 2 entries */
    CHECK_EQ_INT(ks_pack_open(&pack, buf, size), KS_PACK_BAD_INDEX);
    size = build_pack(buf);
    put_u32le(buf + 4, 0xFFFFFFFFu);
    CHECK_EQ_INT(ks_pack_open(&pack, buf, size), KS_PACK_BAD_INDEX);
    /* A count of MAX_ENTRIES in a 60 byte file: the count is allowed, the index is not. */
    size = build_pack(buf);
    put_u32le(buf + 4, MAX_ENTRIES);
    CHECK_EQ_INT(ks_pack_open(&pack, buf, size), KS_PACK_BAD_INDEX);

    /* ---- the index does not fit inside the file ---- */
    /* One byte of the index is cut off. */
    size = build_pack(buf);
    CHECK_EQ_INT(ks_pack_open(&pack, buf, size - 1u), KS_PACK_BAD_INDEX);
    /* index_offset near 0xFFFFFFFF, far behind the end of the file. */
    size = build_pack(buf);
    put_u32le(buf + 8, 0xFFFFFFF0u);
    CHECK_EQ_INT(ks_pack_open(&pack, buf, size), KS_PACK_BAD_INDEX);
    /* entry_count whose * 16 overflows 32 bits, so a u32 sum would look like it fits. */
    size = build_pack(buf);
    put_u32le(buf + 4, 0x10000000u); /* 0x10000000 * 16 == 0 in 32 bits */
    CHECK_EQ_INT(ks_pack_open(&pack, buf, size), KS_PACK_BAD_INDEX);
    /* One row of index behind a header-only file. */
    uint8_t short_one[HEADER_SIZE];
    memset(short_one, 0, sizeof(short_one));
    memcpy(short_one, "KPK1", 4);
    put_u32le(short_one + 4, 1);
    put_u32le(short_one + 8, HEADER_SIZE);
    put_u32le(short_one + 12, 0);
    CHECK_EQ_INT(ks_pack_open(&pack, short_one, sizeof(short_one)), KS_PACK_BAD_INDEX);

    /* ---- the index bytes do not match index_crc ---- */
    size = build_pack(buf);
    buf[INDEX_AT] ^= 0xFFu; /* one index byte changed, the header still agrees */
    CHECK_EQ_INT(ks_pack_open(&pack, buf, size), KS_PACK_BAD_CRC);
    size = build_pack(buf);
    buf[PACK_SIZE - 1] ^= 0x01u; /* the last index byte changed */
    CHECK_EQ_INT(ks_pack_open(&pack, buf, size), KS_PACK_BAD_CRC);
    size = build_pack(buf);
    put_u32le(buf + 12, ks_crc32(0, "beta", ENTRY2_LEN)); /* index_crc of the wrong bytes */
    CHECK_EQ_INT(ks_pack_open(&pack, buf, size), KS_PACK_BAD_CRC);
    /* A zero-entry pack has an index of zero bytes, whose CRC-32 is 0. */
    memset(empty, 0, sizeof(empty));
    memcpy(empty, "KPK1", 4);
    put_u32le(empty + 4, 0);
    put_u32le(empty + 8, HEADER_SIZE);
    put_u32le(empty + 12, 1);
    CHECK_EQ_INT(ks_pack_open(&pack, empty, sizeof(empty)), KS_PACK_BAD_CRC);

    return ks_test_result();
}
