#ifndef _DRIVER_TYPES_H
#define _DRIVER_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

#include <types.h>
#include <property_types.h>

typedef struct driver_fops_s driver_fops_t;
typedef struct driver_ctx_s driver_ctx_t;
typedef struct driver_s driver_t;

typedef enum {
    DRV_CORE,
    DRV_DIO,
    DRV_GPIO,
    DRV_GPIO_PORT,
    DRV_GPIO_PIN,
    DRV_I2C,
    DRV_SPI,
    DRV_QSPI,
    DRV_TEST,
} driver_types_t;

struct driver_ctx_s {
    driver_t* parent;                               //Parent of this driver.
    const size_t open_max;                          //Max amount of open operations. Fixed
    size_t open_cntr;                               // Current number of opens.
    const property_list_t properties;               // Driver properties. Fixed.
    const char* reg_name;                           // Name under which the driver is registered.
};

struct driver_fops_s {
    int (*reg_drv)(driver_t* base_driver, const char* name, driver_t* driver);
    int (*dereg_drv)(driver_t* base_driver, driver_t* driver);
    driver_t* (*open)(driver_t* base_driver, const char* name);
    int (*close)(driver_t* driver);
    ssize_t (*read)(driver_t* driver, void* buffer, size_t count);
    ssize_t (*write)(driver_t* driver, const void* buffer, size_t count);
    int (*ioctl)(driver_t* driver, size_t id, void* param);
    size_t (*get_properties)(driver_t* driver);
    property_t* (*get_property)(driver_t* driver, size_t id);
};

struct driver_s {
    const char* const name;                         //Name of the driver. Fixed.
    const driver_types_t type;                      //Type of the driver. Fixed.
    const driver_fops_t* const fops;                //File operations. Fixed.
    driver_ctx_t* const ctx;                        //Internal data. Pointer fixed, content variable.
    void* const user;                               //User data. Pointer fixed, content variable.
};

#endif // _DRIVER_TYPES_H_