#include <driver.h>
#include <unity.h>
#include <drv_core.h>
#include <drv_dio.h>
#include <test_registry.h>
#include <test_driver.h>

void setUp(void) {
    test_registry_setUp();
}     // optional
void tearDown(void) {
    test_registry_tearDown();
}  // optional

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_driver_run_all);
    RUN_TEST(test_registry_run_all);
    return UNITY_END();
}