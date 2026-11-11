#include "crc32.h"
#include "ks_test.h"

int main(void) {
    /* Standard check value for CRC-32. */
    CHECK_EQ_INT(ks_crc32(0, "123456789", 9), 0xCBF43926u);
    CHECK_EQ_INT(ks_crc32(0, "", 0), 0);

    /* Feeding the data in two pieces gives the same result as one call. */
    uint32_t partial = ks_crc32(0, "1234", 4);
    CHECK_EQ_INT(ks_crc32(partial, "56789", 5), 0xCBF43926u);

    /* The string helper hashes everything before the terminating zero. */
    CHECK_EQ_INT(ks_crc32_str("123456789"), 0xCBF43926u);
    CHECK_EQ_INT(ks_crc32_str(""), 0);
    CHECK_EQ_INT(ks_crc32_str("bg/school_gate.img"), ks_crc32(0, "bg/school_gate.img", 18));

    return ks_test_result();
}
