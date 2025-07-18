#include "drv_dio.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <types.h>

#include <registry.h>

/*
 * LOCAL Prototypes
 */
static int drv_dio_reg_drv(driver_t* base_driver, const char* name, driver_t* driver);
static int drv_dio_dereg_drv(driver_t* base_driver, driver_t* driver);
static driver_t* drv_dio_open(driver_t* base_driver, const char* name);
static int drv_dio_close(driver_t* driver);
static ssize_t drv_dio_read(driver_t* driver, void* buffer, size_t count);
static ssize_t drv_dio_write(driver_t* driver, const void* buffer, size_t count);
static int drv_dio_ioctl(driver_t* driver, size_t id, void* param);
static size_t drv_dio_get_properties(driver_t* driver);
static property_t* drv_dio_get_property(driver_t* driver, size_t id);

/*
 * LOCAL Variables 
 */

static registry_t drv_dio_params = {
    NULL,
    0,
    0
};

const driver_fops_t drv_dio_fops = {
        .reg_drv = drv_dio_reg_drv,
        .dereg_drv = drv_dio_dereg_drv,
        .open = drv_dio_open,
        .close = drv_dio_close,
        .read = drv_dio_read,
        .write = drv_dio_write,
        .ioctl = drv_dio_ioctl,
        .get_properties = drv_dio_get_properties,
        .get_property = drv_dio_get_property,

};

static const property_t drv_dio_properties[] = {
};

static driver_ctx_t drv_dio_ctx = {
    .open_cntr = 0,
    .open_max = 1,
    .parent = NULL,
    .properties = {
        .count = 1,
        .list = drv_dio_properties,
    },
    .reg_name = "dio"
};

static const driver_t drv_dio_config = {
    .name = "dio",
    .type = DRV_DIO,
    .fops = &drv_dio_fops,
    .ctx  = &drv_dio_ctx,
    .user = (void* const)&drv_dio_params,
};

const driver_t const* drv_dio = &drv_dio_config;

/*
 * LOCAL Functions
 */
static int drv_dio_reg_drv(driver_t* base_driver, const char* name, driver_t* driver) {
    // base_driver kann nicht null sein, da von drv_register() drv_dio_reg_drv über base_driver->fop aufgerufen wird.
    // name und driver sind schon vorab auf Gültigkeit geprüft.

    // Core-Treiber darf nicht registriert werden.
    if (driver->type == DRV_DIO) {
        errno = EINVAL;
        return -1;
    }

    registry_t* registry = (registry_t*) base_driver->user;
    // Prüfe registry. Sollte eigentlich nie NULL sein, da statisch definiert.
    if (registry == NULL) {
        errno = ENOSYS;
        return -1;
    }

    return registry_add_driver(registry, driver);

}

static int drv_dio_dereg_drv(driver_t* base_driver, driver_t* driver) {
    ssize_t index;

    registry_t* registry = (registry_t*) base_driver->user;
    // Prüfe registry. Sollte eigentlich nie NULL sein, da statisch definiert.
    if (registry == NULL) {
        errno = ENOSYS;
        return -1;
    }

    return registry_remove_driver(registry, driver);
}

static driver_t* drv_dio_open(driver_t* base_driver, const char* name) {

    registry_t* registry = (registry_t*) base_driver->user;
    // Prüfe registry. Sollte eigentlich nie NULL sein, da statisch definiert.
    if (registry == NULL) {
        errno = ENOSYS;
        return NULL;
    }
    
    // Suche Treiberhandle
    driver_t* driver = registry_get_driver_by_name(registry, name);
    if ((driver == NULL) || (driver->ctx == NULL)) {
        errno = ENOENT;
        return NULL;
    }

    // Erhöhe die Anzahl der geöffneten handles
    if ((driver->ctx->open_max == 0) || (driver->ctx->open_cntr < driver->ctx->open_max)) {
        driver->ctx->open_cntr++;
        return driver;
    }

    // Wenn hier, dann ist die max. Anzahl der gleichzeitigen Öffnungen überschritten.
    errno = EBUSY;
    return NULL;
}

static int drv_dio_close(driver_t* driver) {
    // Parametercheck für driver ist nicht notwendig, da schon von drv_close geprüft.

    // Prüfe, ob schon alles geschlossen ist.
    if (driver->ctx->open_cntr) {
        driver->ctx->open_cntr--;
        return 0;
    }
    else {
        errno = EBADF;
        return -1;
    }
}

static ssize_t drv_dio_read(driver_t* driver, void* buffer, size_t count) {
    errno = ENOTSUP;
    return -1;
}

static ssize_t drv_dio_write(driver_t* driver, const void* buffer, size_t count) {
    errno = ENOTSUP;
    return -1;
}

static int drv_dio_ioctl(driver_t* driver, size_t id, void* param) {
    errno = ENOTSUP;
    return -1;
}

static size_t drv_dio_get_properties(driver_t* driver) {
    errno = ENOTSUP;
    return -1;
}

static property_t* drv_dio_get_property(driver_t* driver, size_t id) {
    errno = ENOTSUP;
    return NULL;
}
