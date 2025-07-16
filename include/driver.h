#ifndef _DRIVER_H_
#define _DRIVER_H_
#include <driver_types.h>

int drv_register(driver_t* base_driver, const char* name, driver_t* driver);
int drv_deregister(driver_t* base_driver, driver_t* driver);
driver_t* drv_open(driver_t* base_driver, const char* name);
int drv_close(driver_t* drv);
ssize_t drv_read(driver_t* drv, void* buffer, size_t buffer_len);
ssize_t drv_write(driver_t* drv, const void* buffer, size_t buffer_len);
int drv_ioctl(driver_t*, size_t id, void* param);

#endif //_DRIVER_H_