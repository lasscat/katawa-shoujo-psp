#include "bytes.h"
#include "ks_test.h"

int main(void) {
    const uint8_t data[] = {0x34, 0x12, 0x78, 0x56, 0xFF, 0xFF, 0xFF, 0xFF, 0x00};

    CHECK_EQ_INT(ks_read_u16le(data), 0x1234);
    CHECK_EQ_INT(ks_read_u16le(data + 2), 0x5678);
    CHECK_EQ_INT(ks_read_u16le(data + 4), 0xFFFF);

    CHECK_EQ_INT(ks_read_u32le(data), 0x56781234u);
    CHECK_EQ_INT(ks_read_u32le(data + 4), 0xFFFFFFFFu);

    /* An odd address must work too. */
    CHECK_EQ_INT(ks_read_u32le(data + 1), 0xFF567812u);
    CHECK_EQ_INT(ks_read_u16le(data + 7), 0x00FF);

    return ks_test_result();
}
