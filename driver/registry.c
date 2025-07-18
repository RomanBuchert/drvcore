/**
 * @file    registry.c
 * @brief   Driver registry implementation for (sub)driver management.
 *
 * @details
 * This module provides shared logic for adding, removing and querying
 * (sub)drivers in dynamic driver hierarchies.
 * It supports dynamic allocation and index-based lookup.
 *
 * @warning
 * Parameters are not validated internally. Callers must ensure correctness!
 * Callers must also ensure, that registry in initialized with NULL/0 on all parameters.
 *
 * @author  Roman Buchert <roman.buchert@googlemail.com>
 * @date    2025-07-17
 * @version 0.1
 */

 /*
 * INCLUDEs
 */
#include "registry.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*
 * Global Functions
 */
/**
 * @brief registry_add_driver: Add a driver to the list.
 * If the list is empty or full, it will be expanded automaticly.
 * It also checks, if the driver is already in the list and revode adding it.
 * 
 * A driver will only be registered, if
 * * A driver with the same registered name
 * * A driver with the same driver_t* driver
 * does not exist
 *
 * @param (registry_t*) registry: List, to add the driver to.
 * @param (const driver_t* const) driver: Driver to be added.
 * 
 * @return (int) 0: Success, -1: Failed. For reason, see errno-variable.
 */
int registry_add_driver(registry_t* registry, const driver_t* const driver) {
    // Parameter check
    if ((registry == NULL) || (driver == NULL)) {
        errno = EINVAL;
        return -1;
    }

    /*
     * Resize / initialize the drivers list, if neccessary.
     * It makes it much easier to allocate memory first and then check whether the driver is already registered.
     * The overhead of increasing the list size even though the driver isn't registered is negligible.
     */
    if (registry->driver_list_size == registry->driver_list_used) {
        size_t new_size = registry->driver_list_size + REGISTRY_EXPAND_SIZE;
        driver_t** new_list = realloc(registry->driver_list, new_size * sizeof(driver_t*));
        if (new_list == NULL) {
            errno = ENOMEM;
            return -1;
        }
        // Clear new allocated memory.
        memset(new_list + registry->driver_list_size, 0, (new_size - registry->driver_list_size) * sizeof(driver_t*));
        registry->driver_list = new_list;
        registry->driver_list_size = new_size;
    }

    // Doublication check
    if ((registry_get_index_by_reg_name(registry, driver->ctx->reg_name) != -1) ||
        (registry_get_index_by_driver(registry, driver) != -1)) {
        errno = EEXIST;
        return -1;
    }

    /*
     * Get free index.
     * Since we previously checked whether the list was full and expanded it if necessary,
     * we ensure that there is a free space in the list.
     */
    ssize_t free_index = registry_get_free_index(registry);
    registry->driver_list[free_index] = (driver_t*)driver;
    registry->driver_list_used++;
    return 0;
}

/**
 * @brief registry_remove_driver: Remove a driver from the list.
 * 
 *
 * @param (registry_t*) registry: List, to remove the driver from.
 * @param (const driver_t* const) driver: Driver to be removed.
 * 
 * @return (int) 0: Success, -1: Failed. For reason see errno-variable.
 */
int registry_remove_driver(registry_t* registry, const driver_t* const driver) {
    // Parameter check
    if ((registry == NULL) || (driver == NULL)) {
        errno = EINVAL;
        return -1;
    }

    // Search for driver
    ssize_t index = registry_get_index_by_driver(registry, driver);
    if (index < 0) {
        errno = ENOENT;
        return -1;
    }

    // Remove the driver from the list.
    registry->driver_list[index] = NULL;
    registry->driver_list_used--;           //Since the driver is only removed if it has been registered, this ensures that "driver_list_used" is always > 0.
    return 0;
}

/**
 * @brief registry_free_registry: Free the allocated registry.
 * Will only be freed, if empty (All entries in list are nullpointer).
 * 
 * @param (registry_t*) registry: List to be freed.
 * 
 * @return (int): 0: Success, -1: Failed. For reason see errno-variable.
 */
int registry_free_registry(registry_t* registry) {
    // Parameter check
    if ((registry == NULL)) {
        errno = EINVAL;
        return -1;
    }

    // Check, if list is empty
    if (registry->driver_list_used > 0) {
        errno = ENOTEMPTY;
        return -1;
    }

    // Free driver list.
    // TODO: Should we handle, if it is already freed? Is this even possible?
    if (registry->driver_list != NULL) {
        free(registry->driver_list);
        registry->driver_list = NULL;
    }

    return 0;
}

/**
 * @brief registry_get_driver_by_name: Get driver handle by name.
 * 
 * @param (const registry_t* const) registry: Pointer to struct, where the drivers are stored in.
 * @param (const char* const ) name: Name of the driver to get.
 * 
 * @return (driver_t*): NULL: Driver not found; other: handle to the requested driver.
 */
driver_t* registry_get_driver_by_name(const registry_t* const registry, const char* const name) {
    for(size_t i = 0; i < registry->driver_list_size; i++) {
        if (registry->driver_list[i] == NULL) {
            continue;
        }
        if ((strlen(registry->driver_list[i]->name) == strlen(name)) &&
            (strcmp(registry->driver_list[i]->name, name) == 0)) {
                return registry->driver_list[i];
        }
    }
    return NULL;
}

/**
 * @brief registry_get_driver_by_reg_name: Get driver handle by registered name.
 * 
 * @param (const registry_t* const) registry: Pointer to struct, where the drivers are stored in.
 * @param (const char* const ) reg_name: Registered name of the driver to get.
 * 
 * @return (driver_t*): NULL: Driver not found; other: handle to the requested driver.
 */
driver_t* registry_get_driver_by_reg_name(const registry_t* const registry, const char* const reg_name) {
    for(size_t i = 0; i < registry->driver_list_size; i++) {
        if ((registry->driver_list[i] == NULL) ||(registry->driver_list[i]->ctx == NULL)) {
            continue;
        }
        if ((strlen(registry->driver_list[i]->ctx->reg_name) == strlen(reg_name)) &&
            (strcmp(registry->driver_list[i]->ctx->reg_name, reg_name) == 0)) {
                return registry->driver_list[i];
        }
    }
    return NULL;
}

/**
 * @brief registry_get_driver_by_index: Get driver handle by index.
 * 
 * @param (const registry_t* const) registry: Pointer to struct, where the drivers are stored in.
 * @param (size_t) index: Index of the driver to get.
 * 
 * @return (driver_t*): NULL: Driver not found; other: handle to the requested driver.
 */
driver_t* registry_get_driver_by_index(const registry_t* const registry, size_t index) {
    if (index < registry->driver_list_size) {
        return registry->driver_list[index];
    }

    return NULL;
}

/**
 * @brief registry_get_index_by_name: Get the index of the driver by the drivers name.
 * 
 * @param (const registry_t* const) registry: Pointer to struct, where the drivers are stored in.
 * @param (const char* const) name: Name of the driver to get.
 * 
 * @return (ssize_t): -1: Driver not found; other: Index of the driver
 */
ssize_t registry_get_index_by_name(const registry_t* const registry, const char* const name) {
    ssize_t i;
    for (i = 0; i < registry->driver_list_size; i++) {
        if (registry->driver_list[i] == NULL) {
            continue;
        }
        if ((strlen(registry->driver_list[i]->name) == strlen(name)) &&
            (strcmp(registry->driver_list[i]->name, name) == 0)) {
                return i;
        }
    }
    return -1;
}

/**
 * @brief registry_get_index_by_reg_name: Get the index of the driver by the drivers registered name.
 * 
 * @param (const registry_t* const) registry: Pointer to struct, where the drivers are stored in.
 * @param (const char* const) reg_name: Registered name of the driver to get.
 * 
 * @return (ssize_t): -1: Driver not found; other: Index of the driver
 */
ssize_t registry_get_index_by_reg_name(const registry_t* const registry, const char* const reg_name) {
    ssize_t i;
    for (i = 0; i < registry->driver_list_size; i++) {
        if ((registry->driver_list[i] == NULL) || (registry->driver_list[i]->ctx == NULL)) {
            continue;
        }
        if ((strlen(registry->driver_list[i]->ctx->reg_name) == strlen(reg_name)) &&
            (strcmp(registry->driver_list[i]->ctx->reg_name, reg_name) == 0)) {
                return i;
        }
    }
    return -1;
}

/**
 * @brief registry_get_index_by_driver: Get the index of the driver by the drivers handle.
 * 
 * @param (const registry_t* const) registry: Pointer to struct, where the drivers are stored in.
 * @param (const driver_t* const) name: Name of the driver to get.
 * 
 * @return (ssize_t): -1: Driver not found; other: Index of the driver
 */
ssize_t registry_get_index_by_driver(const registry_t* const registry, const driver_t* const driver) {
    ssize_t i;
    for (i = 0; i < registry->driver_list_size; i++) {
        if (registry->driver_list[i] == driver) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief registry_get_free_index: Search for a free index in the drivers storage list.
 * 
 * @param (const registry_t* const) registry: Pointer to struct, where the drivers are stored in.
 * 
 * @return (ssize_t): -1: No free index; other Free Index.
 */
ssize_t registry_get_free_index(const registry_t* const registry) {
    for(size_t i = 0; i < registry->driver_list_size; i++) {
        if (registry->driver_list[i] == NULL) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief regsitry_get_space: Return the number of free elements.
 * 
 * @param (const registry_t* const) registry: Pointer to struct, where the drivers are stored in.
 * 
 * @return (ssize_t): Number of free elements in registry list
 */
ssize_t registry_get_space(const registry_t* const registry) {
    // Parameter check
    if (registry == NULL) {
        errno = EINVAL;
        return -1;
    }

    ssize_t space = 0;
    for (size_t i = 0; i < registry->driver_list_size; i++) {
        if (registry->driver_list[i] == NULL) {
            space++;
        }
    }

    return space;
}

/**
 * @brief registry_get_size: Returns the number of elements that can be stored in list.
 * 
 * @param (const registry_t* const) registry: Pointer to struct, where the drivers are stored in.
 *
 * @return (ssize_t): Number of elements that can be stored in list.
 */
ssize_t registry_get_size(const registry_t* const registry) {
    // Parameter check
    if (registry == NULL) {
        errno = EINVAL;
        return -1;
    }

    return (ssize_t) registry->driver_list_size;
}
