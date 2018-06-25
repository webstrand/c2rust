// RUN: %clang_xcheck -O2 -o %t %s %xcheck_runtime %fakechecks
// RUN: %t 2>&1 | FileCheck %s

#include <stdio.h>
#include <stdint.h>

#include <cross_checks.h>

#define DEFINE_TYPE_TEST(Type)  Type test_ ## Type(Type x) { return x; }
#define CALL_TYPE_TEST(Type, x) test_ ## Type(x)

DEFINE_TYPE_TEST(uint8_t)
DEFINE_TYPE_TEST(uint16_t)
DEFINE_TYPE_TEST(uint32_t)
DEFINE_TYPE_TEST(uint64_t)
DEFINE_TYPE_TEST(int8_t)
DEFINE_TYPE_TEST(int16_t)
DEFINE_TYPE_TEST(int32_t)
DEFINE_TYPE_TEST(int64_t)
DEFINE_TYPE_TEST(_Bool)
DEFINE_TYPE_TEST(float)
DEFINE_TYPE_TEST(double)

int main() {
// CHECK: XCHECK(1):2090499946/0x7c9a7f6a
    CALL_TYPE_TEST(uint8_t,  0x12);
// CHECK: XCHECK(1):2506066287/0x955f896f
// CHECK: XCHECK(2):2506066287/0x955f896f
// CHECK: XCHECK(4):18/0x00000012
    CALL_TYPE_TEST(uint16_t, 0x1234);
// CHECK: XCHECK(1):1095512062/0x414c2ffe
// CHECK: XCHECK(2):1095512062/0x414c2ffe
// CHECK: XCHECK(4):6510615555426895982/0x5a5a5a5a5a5a486e
    CALL_TYPE_TEST(uint32_t, 0x12345678);
// CHECK: XCHECK(1):1095579580/0x414d37bc
// CHECK: XCHECK(2):1095579580/0x414d37bc
// CHECK: XCHECK(4):13021231110615524044/0xb4b4b4b4a680e2cc
    CALL_TYPE_TEST(uint64_t, 0x123456789abcdefULL);
// CHECK: XCHECK(1):1095689569/0x414ee561
// CHECK: XCHECK(2):1095689569/0x414ee561
// CHECK: XCHECK(4):1021273028302258913/0xe2c4a6886a4c2e1
    CALL_TYPE_TEST(int8_t,  0x12);
// CHECK: XCHECK(1):3777214650/0xe123b8ba
// CHECK: XCHECK(2):3777214650/0xe123b8ba
// CHECK: XCHECK(4):14106333703424951248/0xc3c3c3c3c3c3c3d0
    CALL_TYPE_TEST(int16_t, 0x1234);
// CHECK: XCHECK(1):93735081/0x059648a9
// CHECK: XCHECK(2):93735081/0x059648a9
// CHECK: XCHECK(4):2170205185142295592/0x1e1e1e1e1e1e0c28
    CALL_TYPE_TEST(int32_t, 0x12345678);
// CHECK: XCHECK(1):93802599/0x05975067
// CHECK: XCHECK(2):93802599/0x05975067
// CHECK: XCHECK(4):8680820740331417102/0x787878786a4c2e0e
    CALL_TYPE_TEST(int64_t, 0x123456789abcdefULL);
// CHECK: XCHECK(1):93912588/0x0598fe0c
// CHECK: XCHECK(2):93912588/0x0598fe0c
// CHECK: XCHECK(4):15272154616569601855/0xd3f197b55b791f3f
    CALL_TYPE_TEST(_Bool, 0);
// CHECK: XCHECK(1):3875382191/0xe6fda3af
// CHECK: XCHECK(2):3875382191/0xe6fda3af
// CHECK: XCHECK(4):9765923333140350852/0x8787878787878784
    CALL_TYPE_TEST(_Bool, 1);
// CHECK: XCHECK(1):3875382191/0xe6fda3af
// CHECK: XCHECK(2):3875382191/0xe6fda3af
// CHECK: XCHECK(4):9765923333140350853/0x8787878787878785
    CALL_TYPE_TEST(float,  1.0f);
// CHECK: XCHECK(1):3885192538/0xe793555a
// CHECK: XCHECK(2):3885192538/0xe793555a
// CHECK: XCHECK(4):4340410369336687672/0x3c3c3c3c03bc3c38
    CALL_TYPE_TEST(double, 1.0);
// CHECK: XCHECK(1):3582805695/0xd58d46bf
// CHECK: XCHECK(2):3582805695/0xd58d46bf
// CHECK: XCHECK(4):12206609413550020242/0xa966969696969692
    return 0;
// CHECK: XCHECK(2):2090499946/0x7c9a7f6a
// CHECK: XCHECK(4):8680820740569200758/0x7878787878787876
}
