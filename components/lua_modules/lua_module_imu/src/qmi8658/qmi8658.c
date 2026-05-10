/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "qmi8658.h"

#include <string.h>

/* Helper — single-register read */
static int8_t reg_read(qmi8658_dev_t *dev, uint8_t reg, uint8_t *val)
{
    return dev->read(reg, val, 1, dev->intf_ptr);
}

/* Helper — single-register write */
static int8_t reg_write(qmi8658_dev_t *dev, uint8_t reg, uint8_t val)
{
    return dev->write(reg, &val, 1, dev->intf_ptr);
}

int8_t qmi8658_init(qmi8658_dev_t *dev)
{
    if (dev == NULL || dev->read == NULL || dev->write == NULL) {
        return QMI8658_E_NULL_PTR;
    }

    /* Verify WHO_AM_I */
    uint8_t chip_id = 0;
    int8_t rslt = reg_read(dev, QMI8658_REG_WHO_AM_I, &chip_id);
    if (rslt != QMI8658_OK) {
        return QMI8658_E_COM_FAIL;
    }
    if (chip_id != QMI8658_CHIP_ID) {
        return QMI8658_E_DEV_NOT_FOUND;
    }
    dev->chip_id = chip_id;

    /* CTRL1: disable standby oscillator (bit0=0), enable address auto-increment */
    uint8_t ctrl1 = 0;
    rslt = reg_read(dev, QMI8658_REG_CTRL1, &ctrl1);
    if (rslt != QMI8658_OK) {
        return rslt;
    }
    ctrl1 = (ctrl1 & 0xFE) | QMI8658_CTRL1_ADDR_AI;
    rslt = reg_write(dev, QMI8658_REG_CTRL1, ctrl1);
    if (rslt != QMI8658_OK) {
        return rslt;
    }

    /* CTRL2: accelerometer — 16 G, 1 kHz ODR */
    rslt = reg_write(dev, QMI8658_REG_CTRL2, QMI8658_ASCALE_16G | QMI8658_AODR_1000HZ);
    if (rslt != QMI8658_OK) {
        return rslt;
    }

    /* CTRL3: gyroscope — 512 DPS, 1 kHz ODR */
    rslt = reg_write(dev, QMI8658_REG_CTRL3, QMI8658_GSCALE_512DPS | QMI8658_GODR_1000HZ);
    if (rslt != QMI8658_OK) {
        return rslt;
    }

    /* CTRL6: disable AttitudeEngine */
    rslt = reg_write(dev, QMI8658_REG_CTRL6, 0x00);
    if (rslt != QMI8658_OK) {
        return rslt;
    }

    /* CTRL7: enable accel + gyro in full mode, high-speed clock, no syncSample */
    rslt = reg_write(dev, QMI8658_REG_CTRL7, QMI8658_CTRL7_ACC_GYRO);
    if (rslt != QMI8658_OK) {
        return rslt;
    }

    if (dev->delay_ms) {
        dev->delay_ms(10, dev->intf_ptr);
    }

    return QMI8658_OK;
}

int8_t qmi8658_read_accel_gyro(qmi8658_raw_axes_t *accel,
                               qmi8658_raw_axes_t *gyro,
                               qmi8658_dev_t *dev)
{
    if (accel == NULL || gyro == NULL || dev == NULL) {
        return QMI8658_E_NULL_PTR;
    }

    /* Read 12 bytes starting at AX_L (auto-increment enabled):
     * AX_L AX_H AY_L AY_H AZ_L AZ_H GX_L GX_H GY_L GY_H GZ_L GZ_H */
    uint8_t buf[12] = {0};
    int8_t rslt = dev->read(QMI8658_REG_AX_L, buf, 12, dev->intf_ptr);
    if (rslt != QMI8658_OK) {
        return QMI8658_E_COM_FAIL;
    }

    accel->x = (int16_t)((buf[1] << 8) | buf[0]);
    accel->y = (int16_t)((buf[3] << 8) | buf[2]);
    accel->z = (int16_t)((buf[5] << 8) | buf[4]);

    gyro->x = (int16_t)((buf[7]  << 8) | buf[6]);
    gyro->y = (int16_t)((buf[9]  << 8) | buf[8]);
    gyro->z = (int16_t)((buf[11] << 8) | buf[10]);

    return QMI8658_OK;
}

int8_t qmi8658_read_temperature_raw(int16_t *temp_raw, qmi8658_dev_t *dev)
{
    if (temp_raw == NULL || dev == NULL) {
        return QMI8658_E_NULL_PTR;
    }

    uint8_t buf[2] = {0};
    int8_t rslt = dev->read(QMI8658_REG_TEMP_L, buf, 2, dev->intf_ptr);
    if (rslt != QMI8658_OK) {
        return QMI8658_E_COM_FAIL;
    }

    *temp_raw = (int16_t)((buf[1] << 8) | buf[0]);
    return QMI8658_OK;
}

int8_t qmi8658_get_int_status(uint8_t *int_status, qmi8658_dev_t *dev)
{
    if (int_status == NULL || dev == NULL) {
        return QMI8658_E_NULL_PTR;
    }

    return reg_read(dev, QMI8658_REG_STATUSINT, int_status);
}
