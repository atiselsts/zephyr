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

int icm20948_sensor_init(const struct device *cdev)
{
	int result = 0;
	uint8_t v;
        struct device *dev = (struct device *)cdev;

        ICM_20948_set_bank(dev, 0); // Must be in the right bank
	result = inv_spi_read(REG_WHO_AM_I, &v, 1);
	if (result) {
		return result;
	}

	LOG_DBG("WHO AM I : 0x%X", v);

        result = ICM_20948_sw_reset(dev);
        if (result) {
		return result; 
        }

        /* Need to delay after soft reset */
	k_msleep(50);

        result = ICM_20948_sleep(dev, false);
        if (result) {
		return result; 
        }

        result = ICM_20948_low_power(dev, false);
        if (result) {
		return result; 
        }

        /* result = startup_magnetometer(false); */
        /* if (result) { */
        /*     return result;  */
        /* } */

        result = ICM_20948_set_sample_mode(dev, ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr, ICM_20948_Sample_Mode_Continuous);
        if (result) {
            return result; 
        }

        ICM_20948_fss_t FSS;
        FSS.a = gpm2;   // (ICM_20948_ACCEL_CONFIG_FS_SEL_e)
        FSS.g = dps250; // (ICM_20948_GYRO_CONFIG_1_FS_SEL_e)
        result = ICM_20948_set_full_scale(dev, ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr, FSS);
        if (result) {
            return result; 
        }

        ICM_20948_dlpcfg_t dlpcfg;
        dlpcfg.a = acc_d473bw_n499bw;
        dlpcfg.g = gyr_d361bw4_n376bw5;
        result = ICM_20948_set_dlpf_cfg(dev, ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr, dlpcfg);
        if (result) {
            return result; 
        }

        result = ICM_20948_enable_dlpf(dev, ICM_20948_Internal_Acc, false);
        if (result) {
            return result; 
        }

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
