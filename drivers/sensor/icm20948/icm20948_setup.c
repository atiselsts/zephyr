/*
 * Copyright (c) 2020 TDK Invensense
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/byteorder.h>
#include <drivers/sensor.h>
#include <logging/log.h>

#include "icm20948.h"
#include "icm20948_reg.h"
#include "icm20948_spi.h"

LOG_MODULE_DECLARE(ICM20948, CONFIG_SENSOR_LOG_LEVEL);

int icm20948_sensor_init(const struct device *dev)
{
	int result = 0;
	uint8_t v;

	result = inv_spi_read(REG_WHO_AM_I, &v, 1);

	if (result) {
		return result;
	}

	LOG_ERR("WHO AM I : 0x%X", v);

	/* result = inv_spi_read(REG_DEVICE_CONFIG, &v, 1); */

	/* if (result) { */
	/* 	LOG_DBG("read REG_DEVICE_CONFIG_REG failed"); */
	/* 	return result; */
	/* } */

	/* v |= BIT_SOFT_RESET; */

	/* result = inv_spi_single_write(REG_DEVICE_CONFIG, &v); */

	/* if (result) { */
	/* 	LOG_ERR("write REG_DEVICE_CONFIG failed"); */
	/* 	return result; */
	/* } */

	/* /\* Need at least 10ms after soft reset *\/ */
	/* k_msleep(10); */

	/* v = BIT_GYRO_AFSR_MODE_HFS | BIT_ACCEL_AFSR_MODE_HFS | BIT_CLK_SEL_PLL; */

	/* result = inv_spi_single_write(REG_INTF_CONFIG1, &v); */

	/* if (result) { */
	/* 	LOG_ERR("write REG_INTF_CONFIG1 failed"); */
	/* 	return result; */
	/* } */

	/* v = BIT_EN_DREG_FIFO_D2A | */
	/*     BIT_TMST_TO_REGS_EN | */
	/*     BIT_TMST_EN; */

	/* result = inv_spi_single_write(REG_TMST_CONFIG, &v); */

	/* if (result) { */
	/* 	LOG_ERR("Write REG_TMST_CONFIG failed"); */
	/* 	return result; */
	/* } */

	/* result = inv_spi_read(REG_INTF_CONFIG0, &v, 1); */

	/* if (result) { */
	/* 	LOG_ERR("Read REG_INTF_CONFIG0 failed"); */
	/* 	return result; */
	/* } */

	/* LOG_DBG("Read REG_INTF_CONFIG0 0x%X", v); */

	/* v |= BIT_UI_SIFS_DISABLE_I2C; */

	/* result = inv_spi_single_write(REG_INTF_CONFIG0, &v); */

	/* if (result) { */
	/* 	LOG_ERR("Write REG_INTF_CONFIG failed"); */
	/* 	return result; */
	/* } */

	/* v = 0; */
	/* result = inv_spi_single_write(REG_INT_CONFIG1, &v); */

	/* if (result) { */
	/* 	return result; */
	/* } */

	/* result = inv_spi_single_write(REG_PWR_MGMT0, &v); */

	/* if (result) { */
	/* 	return result; */
	/* } */

	return 0;
}

int icm20948_turn_on_sensor(const struct device *dev)
{
	struct icm20948_data *drv_data = dev->data;

	/* uint8_t v = 0; */
	/* int result = 0; */

	/* if (drv_data->sensor_started) { */
	/* 	LOG_ERR("Sensor already started"); */
	/* 	return -EALREADY; */
	/* } */

	/* /\* icm20948_set_fs(dev, drv_data->accel_sf, drv_data->gyro_sf); *\/ */

	/* /\* icm20948_set_odr(dev, drv_data->accel_hz, drv_data->gyro_hz); *\/ */

	/* v |= BIT_ACCEL_MODE_LNM; */
        /* v |= BIT_GYRO_MODE_LNM; */

	/* result = inv_spi_single_write(REG_PWR_MGMT0, &v); */
	/* if (result) { */
	/* 	return result; */
	/* } */

	/* Accelerometer sensor need at least 10ms startup time
	 * Gyroscope sensor need at least 30ms startup time
	 */
	k_msleep(100);

	/* icm20948_turn_on_fifo(dev); */

	drv_data->sensor_started = true;

	return 0;
}

int icm20948_turn_off_sensor(const struct device *dev)
{
	uint8_t v = 0;
	int result = 0;

	/* result = inv_spi_read(REG_PWR_MGMT0, &v, 1); */

	/* v ^= BIT_ACCEL_MODE_LNM; */
	/* v ^= BIT_GYRO_MODE_LNM; */

	/* result = inv_spi_single_write(REG_PWR_MGMT0, &v); */
	/* if (result) { */
	/* 	return result; */
	/* } */

	/* /\* Accelerometer sensor need at least 10ms startup time */
	/*  * Gyroscope sensor need at least 30ms startup time */
	/*  *\/ */
	/* k_msleep(100); */

	/* icm20948_turn_off_fifo(dev); */

	return 0;
}
