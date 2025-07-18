/**
 * @file    registry.h
 * @brief   Driver registry implementation for (sub)driver management.
 *
 * @details
 * This module provides shared logic for adding, removing and querying
 * (sub)drivers in dynamic driver hierarchies.
 * It supports dynamic allocation and index-based lookup.
 *
 * @warning
 * Parameters are not validated internally. Callers must ensure correctness!
 *
 * @author  Roman Buchert <roman.buchert@googlemail.com>
 * @date    2025-07-17
 * @version 0.1
 */

/*
 * Include Guard
 */
#ifndef _REGISTRY_H_
#define _REGISTRY_H_

/*
 * INCLUDEs
 */
#include <registry_types.h>
#include <driver_types.h>

/*
 * DEFINEs
 */
#define REGISTRY_EXPAND_SIZE    (8U)    /// Expand registry by this size, if necessary.

/*
 * Global Prototypes
 */

/**
 * @brief registry_add_driver: Add a driver to the list.
 * If the list is empty or full, it will be expanded automaticly.
 * It also checks, if the driver is already in the list and revode adding it.
 *
 * @param (registry_t*) registry: List, to add the driver to.
 * @param (const driver_t* const) driver: Driver to be added.
 * 
 * @return (int) 0: Success, -1: Failed. For reason, see errno-variable.
 */
int registry_add_driver(registry_t* registry, const driver_t* const driver);

/**
 * @brief registry_remove_driver: Remove a driver from the list.
 * 
 *
 * @param (registry_t*) registry: List, to remove the driver from.
 * @param (const driver_t* const) driver: Driver to be removed.
 * 
 * @return (int) 0: Success, -1: Failed. For reason see errno-variable.
 */
int registry_remove_driver(registry_t* registry, const driver_t* const driver);

/**
 * @brief registry_free_registry: Free the allocated registry.
 * Will only be freed, if empty (All entries in list are nullpointer).
 * 
 * @param (registry_t*) registry: List to be freed.
 * 
 * @return (int): 0: Success, -1: Failed. For reason see errno-variable.
 */
int registry_free_registry(registry_t* registry);

/**
 * @brief registry_get_driver_by_name: Get driver handle by name.
 * 
 * @param (const registry_t* const) registry: Pointer to struct, where the drivers are stored in.
 * @param (const char* const ) name: Name of the driver to get.
 * 
 * @return (driver_t*): NULL: Driver not found; other: handle to the requested driver.
 */
driver_t* registry_get_driver_by_name(const registry_t* const registry, const char* const name);

/**
 * @brief registry_get_driver_by_index: Get driver handle by index.
 * 
 * @param (const registry_t* const) registry: Pointer to struct, where the drivers are stored in.
 * @param (size_t) index: Index of the driver to get.
 * 
 * @return (driver_t*): NULL: Driver not found; other: handle to the requested driver.
 */
driver_t* registry_get_driver_by_index(const registry_t* const registry, size_t index);

/**
 * @brief registry_get_index_by_name: Get the index of the driver by the drivers name.
 * 
 * @param (const registry_t* const) registry: Pointer to struct, where the drivers are stored in.
 * @param (const char* const) name: Name of the driver to get.
 * 
 * @return (ssize_t): -1: Driver not found; other: Index of the driver
 */
ssize_t registry_get_index_by_name(const registry_t* const registry, const char* const name);

/**
 * @brief registry_get_index_by_driver: Get the index of the driver by the drivers handle.
 * 
 * @param (const registry_t* const) registry: Pointer to struct, where the drivers are stored in.
 * @param (const driver_t* const) name: Name of the driver to get.
 * 
 * @return (ssize_t): -1: Driver not found; other: Index of the driver
 */
ssize_t registry_get_index_by_driver(const registry_t* const registry, const driver_t* const driver);

/**
 * @brief registry_get_free_index: Search for a free index in the drivers storage list.
 * 
 * @param (const registry_t* const) registry: Pointer to struct, where the drivers are stored in.
 * 
 * @return (ssize_t): -1: No free index; other Free Index.
 */
ssize_t registry_get_free_index(const registry_t* const registry);

/**
 * @brief regsitry_get_space: Return the number of free elements.
 * 
 * @param (const registry_t* const) registry: Pointer to struct, where the drivers are stored in.
 * 
 * @return (ssize_t): Number of free elements in registry list
 */
ssize_t registry_get_space(const registry_t* const registry);

/**
 * @brief registry_get_size: Returns the number of elements that can be stored in list.
 * 
 * @param (const registry_t* const) registry: Pointer to struct, where the drivers are stored in.
 *
 * @return (ssize_t): Number of elements that can be stored in list.
 */
ssize_t registry_get_size(const registry_t* const registry);

#endif //_DRIVER_REGISTRY_H_
