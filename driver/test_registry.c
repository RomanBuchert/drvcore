#include "unity.h"
#include "registry.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// ---- Dummy-Kontext und Treiber ----
typedef struct {
    const char* reg_name;
} dummy_ctx_t;

static dummy_ctx_t ctx1 = { .reg_name = "reg_drv1" };
static dummy_ctx_t ctx2 = { .reg_name = "reg_drv2" };

static driver_t drv1 = { .name = "drv1", .ctx = (void*)&ctx1 };
static driver_t drv2 = { .name = "drv2", .ctx = (void*)&ctx2 };

// ---- Testobjekt ----
static registry_t reg;


void test_registry_setUp(void)
{
//    memset(&reg, 0, sizeof(registry_t));
}

void test_registry_tearDown(void)
{
//    free(reg.driver_list);
//    memset(&reg, 0, sizeof(registry_t));
}

// ---- registry_add_driver ----
void test_add_valid_driver_should_succeed(void)
{
    TEST_ASSERT_EQUAL_INT(0, registry_add_driver(&reg, &drv1));
    TEST_ASSERT_EQUAL_INT(1, reg.driver_list_used);
}

void test_add_duplicate_driver_should_fail(void)
{
    registry_add_driver(&reg, &drv1);
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, registry_add_driver(&reg, &drv1));
    TEST_ASSERT_EQUAL_INT(EEXIST, errno);
}

void test_add_driver_with_null_args_should_fail(void)
{
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, registry_add_driver(NULL, &drv1));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);

    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, registry_add_driver(&reg, NULL));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

// ---- registry_remove_driver ----
void test_remove_existing_driver_should_succeed(void)
{
    registry_add_driver(&reg, &drv1);
    TEST_ASSERT_EQUAL_INT(0, registry_remove_driver(&reg, &drv1));
    TEST_ASSERT_EQUAL_INT(0, reg.driver_list_used);
    TEST_ASSERT_NULL(reg.driver_list[0]);
}

void test_remove_nonexistent_driver_should_fail(void)
{
    registry_add_driver(&reg, &drv1);
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, registry_remove_driver(&reg, &drv2));
    TEST_ASSERT_EQUAL_INT(ENOENT, errno);
}

void test_remove_with_null_args_should_fail(void)
{
    registry_add_driver(&reg, &drv1);
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, registry_remove_driver(NULL, &drv1));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);

    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, registry_remove_driver(&reg, NULL));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

// ---- registry_free_registry ----
void test_free_empty_registry_should_succeed(void)
{
    registry_add_driver(&reg, &drv1);
    registry_remove_driver(&reg, &drv1);
    TEST_ASSERT_EQUAL_INT(0, registry_free_registry(&reg));
    TEST_ASSERT_NULL(reg.driver_list);
}

void test_free_nonempty_registry_should_fail(void)
{
    registry_add_driver(&reg, &drv1);
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, registry_free_registry(&reg));
    TEST_ASSERT_EQUAL_INT(ENOTEMPTY, errno);
}

void test_free_null_registry_should_fail(void)
{
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, registry_free_registry(NULL));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

// ---- registry_get_driver_by_name ----
void test_get_driver_by_name_should_return_correct_pointer(void)
{
    registry_add_driver(&reg, &drv1);
    driver_t* found = registry_get_driver_by_name(&reg, "drv1");
    TEST_ASSERT_EQUAL_PTR(&drv1, found);
}

void test_get_driver_by_name_should_return_null_on_not_found(void)
{
    registry_add_driver(&reg, &drv1);
    TEST_ASSERT_NULL(registry_get_driver_by_name(&reg, "nonexistent"));
}

// ---- registry_get_driver_by_reg_name ----
void test_get_driver_by_reg_name_should_return_correct_pointer(void)
{
    registry_add_driver(&reg, &drv1);
    driver_t* found = registry_get_driver_by_reg_name(&reg, "reg_drv1");
    TEST_ASSERT_EQUAL_PTR(&drv1, found);
}

void test_get_driver_by_reg_name_should_return_null_on_not_found(void)
{
    registry_add_driver(&reg, &drv1);
    TEST_ASSERT_NULL(registry_get_driver_by_reg_name(&reg, "unknown"));
}

// ---- registry_get_driver_by_index ----
void test_get_driver_by_index_should_return_correct_pointer(void)
{
    registry_add_driver(&reg, &drv1);
    TEST_ASSERT_EQUAL_PTR(&drv1, registry_get_driver_by_index(&reg, 0));
}

void test_get_driver_by_index_out_of_bounds_should_return_null(void)
{
    registry_add_driver(&reg, &drv1);
    TEST_ASSERT_NULL(registry_get_driver_by_index(&reg, 10));
}

// ---- registry_get_index_by_name ----
void test_get_index_by_name_should_return_correct_index(void)
{
    registry_add_driver(&reg, &drv1);
    TEST_ASSERT_EQUAL_INT(0, registry_get_index_by_name(&reg, "drv1"));
}

void test_get_index_by_name_should_return_negative_on_not_found(void)
{
    registry_add_driver(&reg, &drv1);
    TEST_ASSERT_EQUAL_INT(-1, registry_get_index_by_name(&reg, "unknown"));
}

// ---- registry_get_index_by_reg_name ----
void test_get_index_by_reg_name_should_return_correct_index(void)
{
    registry_add_driver(&reg, &drv1);
    TEST_ASSERT_EQUAL_INT(0, registry_get_index_by_reg_name(&reg, "reg_drv1"));
}

void test_get_index_by_reg_name_should_return_negative_on_not_found(void)
{
    registry_add_driver(&reg, &drv1);
    TEST_ASSERT_EQUAL_INT(-1, registry_get_index_by_reg_name(&reg, "none"));
}

// ---- registry_get_index_by_driver ----
void test_get_index_by_driver_should_return_correct_index(void)
{
    registry_add_driver(&reg, &drv1);
    registry_add_driver(&reg, &drv2);
    TEST_ASSERT_EQUAL_INT(1, registry_get_index_by_driver(&reg, &drv2));
}

void test_get_index_by_driver_should_return_negative_on_not_found(void)
{
    TEST_ASSERT_EQUAL_INT(-1, registry_get_index_by_driver(&reg, &drv2));
}

// ---- registry_get_free_index ----
void test_get_free_index_should_return_first_free_slot(void)
{
    registry_add_driver(&reg, &drv1);
    registry_add_driver(&reg, &drv2);
    registry_remove_driver(&reg, &drv1);
    TEST_ASSERT_EQUAL_INT(0, registry_get_free_index(&reg));
}

// ---- registry_get_space ----
void test_registry_get_space_should_return_correct_number(void)
{
    registry_add_driver(&reg, &drv1);
    registry_add_driver(&reg, &drv2);
    registry_remove_driver(&reg, &drv1);
    TEST_ASSERT_EQUAL_INT(1, registry_get_space(&reg));
}

void test_registry_get_space_with_null_should_fail(void)
{
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, registry_get_space(NULL));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

// ---- registry_get_size ----
void test_registry_get_size_should_return_size(void)
{
    registry_add_driver(&reg, &drv1);
    ssize_t size = registry_get_size(&reg);
    TEST_ASSERT_TRUE(size >= 1);
}

void test_registry_get_size_with_null_should_fail(void)
{
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, registry_get_size(NULL));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

void test_registry_run_all() {
    // alle Tests aufrufen
#define RUN(x) RUN_TEST(x)
    RUN(test_add_valid_driver_should_succeed);
    RUN(test_add_duplicate_driver_should_fail);
    RUN(test_add_driver_with_null_args_should_fail);
    RUN(test_remove_existing_driver_should_succeed);
    RUN(test_remove_nonexistent_driver_should_fail);
    RUN(test_remove_with_null_args_should_fail);
    RUN(test_free_empty_registry_should_succeed);
    RUN(test_free_nonempty_registry_should_fail);
    RUN(test_free_null_registry_should_fail);
    RUN(test_get_driver_by_name_should_return_correct_pointer);
    RUN(test_get_driver_by_name_should_return_null_on_not_found);
    RUN(test_get_driver_by_reg_name_should_return_correct_pointer);
    RUN(test_get_driver_by_reg_name_should_return_null_on_not_found);
    RUN(test_get_driver_by_index_should_return_correct_pointer);
    RUN(test_get_driver_by_index_out_of_bounds_should_return_null);
    RUN(test_get_index_by_name_should_return_correct_index);
    RUN(test_get_index_by_name_should_return_negative_on_not_found);
    RUN(test_get_index_by_reg_name_should_return_correct_index);
    RUN(test_get_index_by_reg_name_should_return_negative_on_not_found);
    RUN(test_get_index_by_driver_should_return_correct_index);
    RUN(test_get_index_by_driver_should_return_negative_on_not_found);
    RUN(test_get_free_index_should_return_first_free_slot);
    RUN(test_registry_get_space_should_return_correct_number);
    RUN(test_registry_get_space_with_null_should_fail);
    RUN(test_registry_get_size_should_return_size);
    RUN(test_registry_get_size_with_null_should_fail);
#undef RUN
}