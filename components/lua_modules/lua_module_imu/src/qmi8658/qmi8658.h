/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Return codes */
#define QMI8658_OK               INT8_C(0)
#define QMI8658_E_NULL_PTR       INT8_C(-1)
#define QMI8658_E_COM_FAIL       INT8_C(-2)
#define QMI8658_E_DEV_NOT_FOUND  INT8_C(-3)

/* I2C addresses (SA0 pin selects high/low) */
#define QMI8658_I2C_ADDRESS      UINT8_C(0x6B)  /* SA0 low (default) */
#define QMI8658_I2C_ADDRESS_SA0H UINT8_C(0x6A)  /* SA0 high */

/* WHO_AM_I value */
#define QMI8658_CHIP_ID          UINT8_C(0x05)

/* Registers */
#define QMI8658_REG_WHO_AM_I     UINT8_C(0x00)
#define QMI8658_REG_REVISION_ID  UINT8_C(0x01)
#define QMI8658_REG_CTRL1        UINT8_C(0x02)
#define QMI8658_REG_CTRL2        UINT8_C(0x03)
#define QMI8658_REG_CTRL3        UINT8_C(0x04)
#define QMI8658_REG_CTRL5        UINT8_C(0x06)
#define QMI8658_REG_CTRL6        UINT8_C(0x07)
#define QMI8658_REG_CTRL7        UINT8_C(0x08)
#define QMI8658_REG_CTRL9        UINT8_C(0x0A)
#define QMI8658_REG_STATUSINT    UINT8_C(0x2D)
#define QMI8658_REG_TEMP_L       UINT8_C(0x33)
#define QMI8658_REG_TEMP_H       UINT8_C(0x34)
#define QMI8658_REG_AX_L         UINT8_C(0x35)
#define QMI8658_REG_GX_L         UINT8_C(0x3B)

/* CTRL1: enable 2 MHz oscillator (bit0=0) + auto-increment (bit6=1) */
#define QMI8658_CTRL1_ADDR_AI    UINT8_C(0x40)

/* CTRL7: accel+gyro normal mode, high-speed clock, no syncSample */
#define QMI8658_CTRL7_ACC_GYRO   UINT8_C(0x43)

/* Accelerometer scale (CTRL2 bits [6:4]) */
#define QMI8658_ASCALE_2G        UINT8_C(0x00)
#define QMI8658_ASCALE_4G        UINT8_C(0x10)
#define QMI8658_ASCALE_8G        UINT8_C(0x20)
#define QMI8658_ASCALE_16G       UINT8_C(0x30)

/* Accelerometer ODR (CTRL2 bits [3:0]) */
#define QMI8658_AODR_1000HZ      UINT8_C(0x03)

/* Gyroscope scale (CTRL3 bits [6:4]) */
#define QMI8658_GSCALE_512DPS    UINT8_C(0x50)

/* Gyroscope ODR (CTRL3 bits [3:0]) */
#define QMI8658_GODR_1000HZ      UINT8_C(0x03)

typedef int8_t (*qmi8658_read_f)(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);
typedef int8_t (*qmi8658_write_f)(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);
typedef void   (*qmi8658_delay_ms_f)(uint32_t period_ms, void *intf_ptr);

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} qmi8658_raw_axes_t;

typedef struct {
    uint8_t            chip_id;
    void              *intf_ptr;
    qmi8658_read_f     read;
    qmi8658_write_f    write;
    qmi8658_delay_ms_f delay_ms;
} qmi8658_dev_t;

int8_t qmi8658_init(qmi8658_dev_t *dev);
int8_t qmi8658_read_accel_gyro(qmi8658_raw_axes_t *accel,
                               qmi8658_raw_axes_t *gyro,
                               qmi8658_dev_t *dev);
int8_t qmi8658_read_temperature_raw(int16_t *temp_raw, qmi8658_dev_t *dev);
int8_t qmi8658_get_int_status(uint8_t *int_status, qmi8658_dev_t *dev);

#ifdef __cplusplus
}
#endif
