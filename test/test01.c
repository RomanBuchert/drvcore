#include <driver.h>
#include <unity.h>

void setUp(void) {}     // optional
void tearDown(void) {}  // optional

int main(void) {
    UNITY_BEGIN();
    driver_t* core;
    return UNITY_END();
}