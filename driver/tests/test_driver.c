#include "unity.h"
#include "driver.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static int tst_reg_drv(driver_t* base_driver, const char* name, driver_t* driver);
static int tst_dereg_drv(driver_t* base_driver, driver_t* driver);
static driver_t* tst_open(driver_t* base_driver, const char* name);
static int tst_close(driver_t* driver);
static ssize_t tst_read(driver_t* base_driver, void* buffer, size_t count);
static ssize_t tst_write(driver_t* base_driver, const void* buffer, size_t count);
static int tst_ioctl(driver_t* base_driver, size_t id, void* param);
static size_t tst_get_properties(driver_t* driver);
static property_t* tst_get_property(driver_t* driver, size_t id);

// ---- Dummy-Kontext und Treiber ----


// ---- Testobjekt ----
static driver_fops_t tst_fops = {
    .reg_drv = tst_reg_drv,
    .dereg_drv = tst_dereg_drv,
    .open = tst_open,
    .close = tst_close,
    .read = tst_read,
    .write = tst_write,
    .ioctl = tst_ioctl,
    .get_properties = tst_get_properties,
    .get_property = tst_get_property
};

static const property_list_t tst_props = {
    .count = 0,
    .list = NULL
};

static driver_ctx_t tst_ctx = {
    .open_cntr = 0,
    .open_max = 1,
    .parent = NULL,
    .properties = tst_props,
    .reg_name = ""
};

driver_t tst_base = {
    .fops = &tst_fops,
    .ctx = &tst_ctx,
    .name = "TestBaseDriver",
    .type = DRV_TEST,
    .user = NULL,
};

driver_t tst_base_no_fops = {
    .fops = NULL,
    .ctx = &tst_ctx,
    .name = "TestBaseDriver",
    .type = DRV_TEST,
    .user = NULL,
};

driver_t tst_driver = {
    .fops = &tst_fops,
    .ctx = &tst_ctx,
    .name = "TestDriver",
    .type = DRV_TEST,
    .user = NULL,
};

#define TST_BUFFER_SIZE (1024U)
char tst_buffer[TST_BUFFER_SIZE];

// ---- Setup / Cleanup -----
void test_driver_setUp(void)
{
}

void test_driver_tearDown(void)
{
}

// ---- Single Tests ----
// ---- drv_register ----
void test_register_valid_driver_should_succeed(void) {
    TEST_ASSERT_EQUAL_INT(0, drv_register(&tst_base, "BaseDriver", &tst_driver));
    TEST_ASSERT_EQUAL_STRING("BaseDriver", tst_driver.ctx->reg_name);
    TEST_ASSERT_EQUAL_PTR(&tst_base, tst_driver.ctx->parent);
}

void test_register_param_check_should_fail(void) {

    // base_driver == NULL
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, drv_register(NULL, "BaseDriver", &tst_driver));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);

    // name = NULL
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, drv_register(&tst_base, NULL, &tst_driver));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);

    // name ist empty ("")
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, drv_register(&tst_base, "", &tst_driver));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);

    // driver = NULL
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, drv_register(&tst_base, "BaseDriver", NULL));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

void test_register_no_fops_should_fail() {
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, drv_register(&tst_base_no_fops, "BaseDriver", &tst_driver));
    TEST_ASSERT_EQUAL_INT(ENOSYS, errno);
}

void test_register_no_reg_fop_should_fail() {
    errno = 0;
    tst_fops.reg_drv = NULL;
    TEST_ASSERT_EQUAL_INT(-1, drv_register(&tst_base, "BaseDriver", &tst_driver));
    TEST_ASSERT_EQUAL_INT(ENOTSUP, errno);
    tst_fops.reg_drv = tst_reg_drv;

}

// ---- drv_deregister ----
void test_deregister_valid_driver_should_succeed(void) {
    TEST_ASSERT_EQUAL_INT(0, drv_deregister(&tst_base, &tst_driver));
}

void test_deregister_param_check_should_fail(void) {

    // base_driver == NULL
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, drv_deregister(NULL, &tst_driver));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);

    // driver = NULL
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, drv_deregister(&tst_base, NULL));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

void test_deregister_no_fops_should_fail() {
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, drv_deregister(&tst_base_no_fops, &tst_driver));
    TEST_ASSERT_EQUAL_INT(ENOSYS, errno);
}

void test_deregister_no_reg_fop_should_fail() {
    errno = 0;
    tst_fops.dereg_drv = NULL;
    TEST_ASSERT_EQUAL_INT(-1, drv_deregister(&tst_base, &tst_driver));
    TEST_ASSERT_EQUAL_INT(ENOTSUP, errno);
    tst_fops.dereg_drv = tst_dereg_drv;

}

// ---- drv_open ----
void test_open_should_succeed() {
    TEST_ASSERT_EQUAL_PTR(0xCafeBabe, drv_open(&tst_base, "Test"));
}

void test_open_param_check_should_fail() {
    // base_driver == NULL
    errno = 0;
    TEST_ASSERT_EQUAL_PTR(NULL, drv_open(NULL, "Test"));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);

    // name == NULL,
    errno = 0;
    TEST_ASSERT_EQUAL_PTR(NULL, drv_open(&tst_base, NULL));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);

    // name is empty (""),
    errno = 0;
    TEST_ASSERT_EQUAL_PTR(NULL , drv_open(&tst_base, ""));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

void test_open_no_fops_should_fail() {
    errno = 0;
    TEST_ASSERT_EQUAL_PTR(NULL, drv_open(&tst_base_no_fops, "Test"));
    TEST_ASSERT_EQUAL_INT(ENOSYS, errno);
}

void test_open_no_open_fop_should_fail() {
    errno = 0;
    tst_fops.open = NULL;
    TEST_ASSERT_EQUAL_PTR(NULL, drv_open(&tst_base, "Test"));
    tst_fops.open = tst_open;
}

// ---- drv_close ----
void test_close_should_succeed() {
    TEST_ASSERT_EQUAL_INT(0, drv_close(&tst_driver));
}

void test_close_param_check_should_fail() {
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, drv_close(NULL));
    TEST_ASSERT_EQUAL_INT(EBADF, errno);
}

void test_close_no_fops_should_fail() {
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, drv_close(&tst_base_no_fops));
    TEST_ASSERT_EQUAL_INT(ENOSYS, errno);
}

void test_close_no_close_fop_should_fail() {
    errno = 0;
    tst_fops.close = NULL;
    TEST_ASSERT_EQUAL_INT(-1, drv_close(&tst_driver));
    TEST_ASSERT_EQUAL_INT(ENOTSUP, errno);
    tst_fops.close = drv_close;
}

// ---- drv_read ----
void test_read_should_succeed() {
    TEST_ASSERT_LESS_OR_EQUAL_INT(TST_BUFFER_SIZE, drv_read(&tst_driver, tst_buffer, TST_BUFFER_SIZE));
}

void test_read_param_check_should_fail() {
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, drv_read(NULL, tst_buffer, TST_BUFFER_SIZE));
    TEST_ASSERT_EQUAL_INT(EBADF, errno);
}
    
void test_read_no_buffer_but_size_should_fail() {
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, drv_read(&tst_driver, NULL, TST_BUFFER_SIZE));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

void test_read_no_fops_should_fail() {
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, drv_read(&tst_base_no_fops, tst_buffer, TST_BUFFER_SIZE));
    TEST_ASSERT_EQUAL_INT(ENOSYS, errno);
}

void test_read_no_read_fop_should_fail() {
    errno = 0;
    tst_fops.read = NULL;
    TEST_ASSERT_EQUAL_INT(-1, drv_read(&tst_base, tst_buffer, TST_BUFFER_SIZE));
    TEST_ASSERT_EQUAL_INT(ENOTSUP, errno);
    tst_fops.read = tst_read;
}

// ---- drv_write ----
void test_write_should_succeed() {
    TEST_ASSERT_LESS_OR_EQUAL_INT(TST_BUFFER_SIZE, drv_write(&tst_driver, tst_buffer, TST_BUFFER_SIZE));
}

void test_write_param_check_should_fail() {
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, drv_write(NULL, tst_buffer, TST_BUFFER_SIZE));
    TEST_ASSERT_EQUAL_INT(EBADF, errno);
}
    
void test_write_no_buffer_but_size_should_fail() {
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, drv_write(&tst_driver, NULL, TST_BUFFER_SIZE));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

void test_write_no_fops_should_fail() {
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, drv_write(&tst_base_no_fops, tst_buffer, TST_BUFFER_SIZE));
    TEST_ASSERT_EQUAL_INT(ENOSYS, errno);
}

void test_write_no_write_fop_should_fail() {
    errno = 0;
    tst_fops.write = NULL;
    TEST_ASSERT_EQUAL_INT(-1, drv_write(&tst_base, tst_buffer, TST_BUFFER_SIZE));
    TEST_ASSERT_EQUAL_INT(ENOTSUP, errno);
    tst_fops.write = tst_write;
}

// ---- drv_ioctl ----
void test_ioctl_should_succeed() {
    TEST_ASSERT_EQUAL_INT(0, drv_ioctl(&tst_driver, 0, NULL));
}

void test_ioctl_param_check_should_fail() {
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, drv_ioctl(NULL, 0, NULL));
    TEST_ASSERT_EQUAL_INT(EBADF, errno);
}

void test_ioctl_no_fops_should_fail() {
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, drv_ioctl(&tst_base_no_fops, 0, NULL));
    TEST_ASSERT_EQUAL_INT(ENOSYS, errno);
}

void test_ioctl_no_ioctl_fop_should_fail() {
    errno = 0;
    tst_fops.ioctl = NULL;
    TEST_ASSERT_EQUAL_INT(-1, drv_ioctl(&tst_driver, 0, NULL));
    TEST_ASSERT_EQUAL_INT(ENOTSUP, errno);
    tst_fops.ioctl = tst_ioctl;
}

// ---- Run all tests ----
void test_driver_run_all() {
    // alle Tests aufrufen
#define RUN(x) RUN_TEST(x)
    // drv_register
    RUN(test_register_valid_driver_should_succeed);
    RUN(test_register_param_check_should_fail);
    RUN(test_register_no_fops_should_fail);
    RUN(test_register_no_reg_fop_should_fail);
    // drv_deregister
    RUN(test_deregister_valid_driver_should_succeed);
    RUN(test_deregister_param_check_should_fail);
    RUN(test_deregister_no_fops_should_fail);
    RUN(test_deregister_no_reg_fop_should_fail);
    // drv_open
    RUN(test_open_should_succeed);
    RUN(test_open_param_check_should_fail);
    RUN(test_open_no_fops_should_fail);
    RUN(test_open_no_open_fop_should_fail);
    // drv_close
    RUN(test_close_should_succeed);
    RUN(test_close_param_check_should_fail);
    RUN(test_close_no_fops_should_fail);
    RUN(test_close_no_close_fop_should_fail);
    // drv_read
    RUN(test_read_should_succeed);
    RUN(test_read_param_check_should_fail);
    RUN(test_read_no_buffer_but_size_should_fail);
    RUN(test_read_no_fops_should_fail);
    RUN(test_read_no_read_fop_should_fail);
    // drv_write
    RUN(test_write_should_succeed);
    RUN(test_write_param_check_should_fail);
    RUN(test_write_no_buffer_but_size_should_fail);
    RUN(test_write_no_fops_should_fail);
    RUN(test_write_no_write_fop_should_fail);
    // drv_ioctl
    RUN(test_ioctl_should_succeed);
    RUN(test_ioctl_param_check_should_fail);
    RUN(test_ioctl_no_fops_should_fail);
    RUN(test_ioctl_no_ioctl_fop_should_fail);
#undef RUN
}

// ---- Helper functions ----
static int tst_reg_drv(driver_t* base_driver, const char* name, driver_t* driver) {
    return 0;
}

static int tst_dereg_drv(driver_t* base_driver, driver_t* driver) {
    return 0;
}

static driver_t* tst_open(driver_t* base_driver, const char* name) {
    return (driver_t*) 0xCAFEBABE;
}

static int tst_close(driver_t* driver) {
    return 0;
}

static ssize_t tst_read(driver_t* base_driver, void* buffer, size_t count) {
    return count;
}

static ssize_t tst_write(driver_t* base_driver, const void* buffer, size_t count) {
    return count;
}

static int tst_ioctl(driver_t* base_driver, size_t id, void* param) {
    return 0;
}

static size_t tst_get_properties(driver_t* driver) {
    return 10;
}

static property_t* tst_get_property(driver_t* driver, size_t id) {
    return (property_t*)0xCafeBabe;
}
