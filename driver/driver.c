#include <driver.h>
#include <string.h>
#include <errno.h>

/*
 * GLOBAL Functions
 */
int drv_register(driver_t* base_driver, const char* name, driver_t* driver) {
    // Parameter check
    if ((base_driver == NULL) || (name == NULL) || (strlen(name) == 0) || (driver == NULL)) {
        errno = EINVAL;
        return -1;
    }

    if (driver->ctx != NULL) {
        driver->ctx->parent = base_driver;      // Overwrite parent explicit!
        driver->ctx->reg_name = name;           // Overwrite name explicit!
    }

    if (base_driver->fops == NULL) {
        errno = ENOSYS;
        return -1;
    }

    if (base_driver->fops->reg_drv != NULL) { 
        return base_driver->fops->reg_drv(base_driver, name, driver);
    }

    errno = ENOTSUP;
    return -1;
}

int drv_deregister(driver_t* base_driver, driver_t* driver) {
    // Parameter check
    if ((base_driver == NULL) || (driver == NULL)) {
        errno = EINVAL;
        return -1;
    }

    if (base_driver->fops == NULL) {
        errno = ENOSYS;
        return -1;
    }

    if (base_driver->fops->dereg_drv != NULL) {
        return base_driver->fops->dereg_drv(base_driver, driver);
    }

    errno = ENOTSUP;
    return -1;

}

driver_t* drv_open(driver_t* base_driver, const char* name) {
    // Parameter check
    if ((base_driver == NULL) || (name == NULL) || (strlen(name) == 0)) {
        errno = EINVAL;
        return NULL;
    }

    if (base_driver->fops == NULL) {
        errno = ENOSYS;
        return NULL;
    }

    if (base_driver->fops->open != NULL) {
        return base_driver->fops->open(base_driver, name);
    }

    errno = ENOTSUP;
    return NULL;
}

int drv_close(driver_t* drv) {
    // Parameter check
    if (drv == NULL) {
        errno = EBADF;
        return -1;
    }

    if (drv->fops == NULL) {
        errno = ENOSYS;
        return -1;
    }

    if (drv->fops->close != NULL) {
        return drv->fops->close(drv);

    }

    errno = ENOTSUP;
    return -1;
}

ssize_t drv_read(driver_t* drv, void* buffer, size_t count) {
    // Parameter check
    if (drv == NULL) {
        errno = EBADF;
        return -1;
    }

    if ((buffer == NULL) && (count > 0)) {
        errno = EINVAL;
        return -1;
    }

    if (drv->fops == NULL) {
        errno = ENOSYS;
        return -1;
    }

    if (drv->fops->read != NULL) {
        return drv->fops->read(drv, buffer, count);
    }

    errno = ENOTSUP;
    return -1;
}

ssize_t drv_write(driver_t* drv, const void* buffer, size_t count) {
    // Parameter check
    if (drv == NULL) {
        errno = EBADF;
        return -1;
    }

    if ((buffer == NULL) && (count > 0)) {
        errno = EINVAL;
        return -1;
    }
    
    if (drv->fops == NULL) {
        errno = ENOSYS;
        return -1;
    }

    if (drv->fops->write != NULL) {
        return drv->fops->write(drv, buffer, count);
    }

    errno = ENOTSUP;
    return -1;
}

int drv_ioctl(driver_t* drv, size_t id, void* param) {
    // Parameter check
    if (drv == NULL) {
        errno = EBADF;
        return -1;
    }

    if (drv->fops == NULL) {
        errno = ENOSYS;
        return -1;
    }

    if (drv->fops->ioctl != NULL) {
        return drv->fops->ioctl(drv, id, param);
    }

    errno = ENOTSUP;
    return -1;
}
