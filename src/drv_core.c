#include "drv_core.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <types.h>

/*
 * TYPEDEFINITIONS
 */
typedef struct drv_core_param_s {
    driver_t** driver_list;
    size_t driver_list_size;
    size_t driver_list_used;
} drv_core_param_t;

/*
 * LOCAL Prototypes
 */
static int drv_core_reg_drv(driver_t* base_driver, const char* name, driver_t* driver);
static int drv_core_dereg_drv(driver_t* base_driver, driver_t* driver);
static driver_t* drv_core_open(driver_t* base_driver, const char* name);
static int drv_core_close(driver_t* driver);
static ssize_t drv_core_read(driver_t* driver, void* buffer, size_t count);
static ssize_t drv_core_write(driver_t* driver, const void* buffer, size_t count);
static int drv_core_ioctl(driver_t* driver, size_t id, void* param);
static size_t drv_core_get_properties(driver_t* driver);
static driver_property_t* drv_core_get_property(driver_t* driver, size_t id);

/**
 * LOCAL Helper Function prototypes
 */
static driver_t* drv_core_get_driver_by_name(const drv_core_param_t* const core_params, const char* const name);
static driver_t* drv_core_get_driver_by_index(const drv_core_param_t* const core_params, size_t index);
static ssize_t drv_core_get_index_by_name(const drv_core_param_t* const core_params, const char* const name);
static ssize_t drv_core_get_index_by_driver(const drv_core_param_t* const core_params, const driver_t* const driver);
static ssize_t drv_core_get_free_index(const drv_core_param_t* const core_params);

/*
 * LOCAL Variables 
 */

static drv_core_param_t drv_core_params = {
    NULL,
    0,
    0
};

const driver_fops_t drv_core_fops = {
        .reg_drv = drv_core_reg_drv,
        .dereg_drv = drv_core_dereg_drv,
        .open = drv_core_open,
        .close = drv_core_close,
        .read = drv_core_read,
        .write = drv_core_write,
        .ioctl = drv_core_ioctl,
        .get_properties = drv_core_get_properties,
        .get_property = drv_core_get_property,

};

static const driver_property_t drv_core_properties[] = {
    { .name = "version1", .property = { .type = TYPE_UNSIGNED | TYPE_CLASS_INT | TYPE_SIZE_32, .uval = 0x00010000 } },
    { .name = "version2", .property = { .type = TYPE_UNSIGNED | TYPE_CLASS_INT | TYPE_SIZE_32, .uval = 0x00010000 } },
    { .name = "version3", .property = { .type = TYPE_UNSIGNED | TYPE_CLASS_INT | TYPE_SIZE_32, .uval = 0x00010000 } },
};

static driver_ctx_t drv_core_ctx = {
    .open_cntr = 0,
    .open_max = 1,
    .parent = NULL,
    .properties = {
        .count = 1,
        .list = drv_core_properties,
    },
    .reg_name = "core"
};

static const driver_t drv_core_config = {
    .name = "core",
    .type = DRV_CORE,
    .fops = &drv_core_fops,
    .ctx  = &drv_core_ctx,
    .user = (void* const)&drv_core_params,
};

const driver_t const* drv_core = &drv_core_config;

/*
 * LOCAL Functions
 */
static int drv_core_reg_drv(driver_t* base_driver, const char* name, driver_t* driver) {
    // base_driver kann nicht null sein, da von drv_register() drv_core_reg_drv über base_driver->fop aufgerufen wird.
    // name und driver sind schon vorab auf Gültigkeit geprüft.

    // Core-Treiber darf nicht registriert werden.
    if (driver->type == DRV_CORE) {
        errno = EINVAL;
        return -1;
    }

    drv_core_param_t* core = (drv_core_param_t*) base_driver->user;
    // Prüfe core. Sollte eigentlich nie NULL sein, da statisch definiert.
    if (core == NULL) {
        errno = ENOSYS;
        return -1;
    }

    // Allokiere bzw. Vergrößere den Speicher, falls notwendig.
    if(core->driver_list_used >= core->driver_list_size) {
        size_t new_size = core->driver_list_size + 8;
        driver_t** new_list = realloc(core->driver_list, new_size * sizeof(driver_t*));
        if (new_list == NULL) {
            errno = ENOMEM;
            return -1;
        }
        // Lösche neu allokierten Speicher.
        memset(new_list + core->driver_list_size, 0, (new_size - core->driver_list_size) * sizeof(driver_t*));
        core->driver_list = new_list;
        core->driver_list_size = new_size;
    }

    // Verhindere doppelte Registrierung.
    if ((drv_core_get_driver_by_name(core, name) != NULL) || 
        (drv_core_get_index_by_driver(core, driver) != -1)) {
        errno = EEXIST;
        return -1;
    }

    // Suche freien Slot
    int index = drv_core_get_free_index(core);
    if ((index < 0) || ((size_t)index >= core->driver_list_size)) {
        errno = ENOMEM;
        return -1;
    }

    // Füge neuen Treiber hinzu.
    core->driver_list[index] = driver;
    core->driver_list_used++;
    return 0;
}

static int drv_core_dereg_drv(driver_t* base_driver, driver_t* driver) {
    ssize_t index;

    drv_core_param_t* core = (drv_core_param_t*) base_driver->user;
    // Prüfe core. Sollte eigentlich nie NULL sein, da statisch definiert.
    if (core == NULL) {
        errno = ENOSYS;
        return -1;
    }

    index = drv_core_get_index_by_driver(core, driver);
    if (index < 0) {
        errno = ENOENT;
        return -1;
    }

    core->driver_list[(size_t)index] = NULL;
    core->driver_list_used--;
    return 0;
}

static driver_t* drv_core_open(driver_t* base_driver, const char* name) {

    drv_core_param_t* core = (drv_core_param_t*) base_driver->user;
    // Prüfe core. Sollte eigentlich nie NULL sein, da statisch definiert.
    if (core == NULL) {
        errno = ENOSYS;
        return NULL;
    }
    
    // Suche Treiberhandle
    driver_t* driver = drv_core_get_driver_by_name(core, name);
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

static int drv_core_close(driver_t* driver) {
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

static ssize_t drv_core_read(driver_t* driver, void* buffer, size_t count) {
    errno = ENOTSUP;
    return -1;
}

static ssize_t drv_core_write(driver_t* driver, const void* buffer, size_t count) {
    errno = ENOTSUP;
    return -1;
}

static int drv_core_ioctl(driver_t* driver, size_t id, void* param) {
    errno = ENOTSUP;
    return -1;
}

static size_t drv_core_get_properties(driver_t* driver) {
    errno = ENOTSUP;
    return -1;
}

static driver_property_t* drv_core_get_property(driver_t* driver, size_t id) {
    errno = ENOTSUP;
    return NULL;
}


/**
 * LOCAL Helper Functions
 * 
 * Da diese Funktionen nur innerhalb von drv_core.c genutzt werden,
 * wird in der Regel auf eine Parameterprüfung verzichtet, da davon ausgegangen wird,
 * dass diese vorher stattgefunden hat.
 */

/**
 * @brief drv_core_get_driver_by_name: Get driver handle by name.
 * 
 * @param (const drv_core_param_t* const) core_params: Pointer to struct, where the drivers are stored in.
 * @param (const char* const ) name: Name of the driver to get.
 * 
 * @return (driver_t*): NULL: Driver not found; other: handle to the requested driver.
 */
 static driver_t* drv_core_get_driver_by_name(const drv_core_param_t* const core_params, const char* const name) {
    for(size_t i = 0; i < core_params->driver_list_size; i++) {
        if (core_params->driver_list[i] == NULL) {
            continue;
        }
        if ((strlen(core_params->driver_list[i]->name) == strlen(name)) &&
            (strcmp(core_params->driver_list[i]->name, name) == 0)) {
                return core_params->driver_list[i];
        }
    }
    return NULL;
}

/**
 * @brief drv_core_get_driver_by_index: Get driver handle by index.
 * 
 * @param (const drv_core_param_t* const) core_params: Pointer to struct, where the drivers are stored in.
 * @param (size_t) index: Index of the driver to get.
 * 
 * @return (driver_t*): NULL: Driver not found; other: handle to the requested driver.
 */
static driver_t* drv_core_get_driver_by_index(const drv_core_param_t* const core_params, size_t index) {
    if (index < core_params->driver_list_size) {
        return core_params->driver_list[index];
    }

    return NULL;
}

/**
 * @brief drv_core_get_index_by_name: Get the index of the driver by the drivers name.
 * 
 * @param (const drv_core_param_t* const) core_params: Pointer to struct, where the drivers are stored in.
 * @param (const char* const) name: Name of the driver to get.
 * 
 * @return (ssize_t): -1: Driver not found; other: Index of the driver
 */
static ssize_t drv_core_get_index_by_name(const drv_core_param_t* const core_params, const char* const name) {
    ssize_t i;
    for (i = 0; i < core_params->driver_list_size; i++) {
        if (core_params->driver_list[i] == NULL) {
            continue;
        }
        if ((strlen(core_params->driver_list[i]->name) == strlen(name)) &&
            (strcmp(core_params->driver_list[i]->name, name) == 0)) {
                return i;
        }
    }
    return -1;
}

/**
 * @brief drv_core_get_index_by_driver: Get the index of the driver by the drivers handle.
 * 
 * @param (const drv_core_param_t* const) core_params: Pointer to struct, where the drivers are stored in.
 * @param (const driver_t* const) name: Name of the driver to get.
 * 
 * @return (ssize_t): -1: Driver not found; other: Index of the driver
 */
static ssize_t drv_core_get_index_by_driver(const drv_core_param_t* const core_params, const driver_t* const driver) {
    ssize_t i;
    for (i = 0; i < core_params->driver_list_size; i++) {
        if (core_params->driver_list[i] == driver) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief drv_core_get_free_index: Search for a free index in the drivers storage list.
 * 
 * @param (const drv_core_param_t* const) core_params: Pointer to struct, where the drivers are stored in.
 * 
 * @return (ssize_t): -1: No free index; other Free Index.
 */
static ssize_t drv_core_get_free_index(const drv_core_param_t* const core_params) {
    for(size_t i = 0; i < core_params->driver_list_size; i++) {
        if (core_params->driver_list[i] == NULL) {
            return i;
        }
    }
    return -1;
}
