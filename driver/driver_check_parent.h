#ifndef _DRIVER_CHECK_PARENT_H_
#define _DRIVER_CHECK_PARENT_H_

#include <stdbool.h>
#include <stddef.h>
#include <driver_types.h> // für driver_t und driver_types_t

// Generische Prüf-Funktion für Parent-Typen
// Wird ggf. auch von generischem Treiber- oder VFS-Code verwendet
static inline bool driver_check_parent_type(driver_t* parent,
                                            const driver_types_t* list,
                                            size_t count) {
    if (!parent || !list) return false;
    for (size_t i = 0; i < count; ++i) {
        if (parent->type == list[i]) return true;
    }
    return false;
}

// Makro für Treiberimplementierungen: definiert eine statische Check-Funktion
// Beispiel: DRV_DEFINE_CHECK_PARENT_FUNC(gpio_pin, DRV_GPIO_PORT);
#define DRV_DEFINE_CHECK_PARENT_FUNC(NAME, ...) \
    static const driver_types_t NAME##_valid_parents[] = { __VA_ARGS__ }; \
    static bool NAME##_check_parent(driver_t* parent) { \
        return driver_check_parent_type(parent, NAME##_valid_parents, \
            sizeof(NAME##_valid_parents) / sizeof(driver_types_t)); \
    }

#endif // _DRIVER_CHECK_PARENT_H_
