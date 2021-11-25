/*
 * Copyright (c) 2020 TDK Invensense
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT invensense_icm20948

#include <drivers/spi.h>
#include <init.h>
#include <sys/byteorder.h>
#include <drivers/sensor.h>
#include <logging/log.h>

#include "icm20948.h"
#include "icm20948_reg.h"
#include "icm20948_setup.h"
#include "icm20948_spi.h"

LOG_MODULE_REGISTER(ICM20948, CONFIG_SENSOR_LOG_LEVEL);

int icm_20948_data_ready(const struct device *cdev)
{
    int retval = 0;
    ICM_20948_INT_STATUS_1_t reg;
    struct device *dev = (struct device *)cdev;

    retval = ICM_20948_set_bank(dev, 0); // Must be in the right bank
    if (retval != 0)
    {
        return retval;
    }
    retval = ICM_20948_execute_r(dev, REG_INT_STATUS_1, (uint8_t *)&reg, sizeof(ICM_20948_INT_STATUS_1_t));
    if (retval != 0)
    {
        return retval;
    }
    if (!reg.RAW_DATA_0_RDY_INT)
    {
        retval = ICM_20948_Stat_NoData;
    }
    return retval;
}

ICM_20948_Status_e ICM_20948_execute_w(const struct device *dev, uint8_t regaddr, uint8_t *pdata, uint32_t len)
{
    // ignore len - always assume 1
    return inv_spi_single_write(regaddr, pdata);
}

ICM_20948_Status_e ICM_20948_execute_r(const struct device *dev, uint8_t regaddr, uint8_t *pdata, uint32_t len)
{
    return inv_spi_read(regaddr, pdata, len);
}

ICM_20948_Status_e ICM_20948_set_bank(struct device *dev, uint8_t bank)
{
  struct icm20948_data *pdev = dev->data;

  if (bank > 3)
  {
    return ICM_20948_Stat_ParamErr;
  } // Only 4 possible banks

  if (bank == pdev->_last_bank) // Do we need to change bank?
    return ICM_20948_Stat_Ok;   // Bail if we don't need to change bank to avoid unnecessary bus traffic

  pdev->_last_bank = bank;   // Store the requested bank (before we bit-shift)
  bank = (bank << 4) & 0x30; // bits 5:4 of REG_BANK_SEL
  return ICM_20948_execute_w(dev, REG_BANK_SEL, &bank, 1);
}

ICM_20948_Status_e ICM_20948_sw_reset(ICM_20948_Device_t *pdev)
{
  ICM_20948_Status_e retval = ICM_20948_Stat_Ok;
  ICM_20948_PWR_MGMT_1_t reg;

  ICM_20948_set_bank(pdev, 0); // Must be in the right bank

  retval = ICM_20948_execute_r(pdev, REG_PWR_MGMT_1, (uint8_t *)&reg, sizeof(ICM_20948_PWR_MGMT_1_t));
  if (retval != ICM_20948_Stat_Ok)
  {
    return retval;
  }

  reg.DEVICE_RESET = 1;

  retval = ICM_20948_execute_w(pdev, REG_PWR_MGMT_1, (uint8_t *)&reg, sizeof(ICM_20948_PWR_MGMT_1_t));
  if (retval != ICM_20948_Stat_Ok)
  {
    return retval;
  }
  return retval;
}

ICM_20948_Status_e ICM_20948_sleep(ICM_20948_Device_t *pdev, bool on)
{
  ICM_20948_Status_e retval = ICM_20948_Stat_Ok;
  ICM_20948_PWR_MGMT_1_t reg;

  ICM_20948_set_bank(pdev, 0); // Must be in the right bank

  retval = ICM_20948_execute_r(pdev, REG_PWR_MGMT_1, (uint8_t *)&reg, sizeof(ICM_20948_PWR_MGMT_1_t));
  if (retval != ICM_20948_Stat_Ok)
  {
    return retval;
  }

  if (on)
  {
    reg.SLEEP = 1;
  }
  else
  {
    reg.SLEEP = 0;
  }

  retval = ICM_20948_execute_w(pdev, REG_PWR_MGMT_1, (uint8_t *)&reg, sizeof(ICM_20948_PWR_MGMT_1_t));
  if (retval != ICM_20948_Stat_Ok)
  {
    return retval;
  }
  return retval;
}

ICM_20948_Status_e ICM_20948_low_power(ICM_20948_Device_t *pdev, bool on)
{
  ICM_20948_Status_e retval = ICM_20948_Stat_Ok;
  ICM_20948_PWR_MGMT_1_t reg;

  ICM_20948_set_bank(pdev, 0); // Must be in the right bank

  retval = ICM_20948_execute_r(pdev, REG_PWR_MGMT_1, (uint8_t *)&reg, sizeof(ICM_20948_PWR_MGMT_1_t));
  if (retval != ICM_20948_Stat_Ok)
  {
    return retval;
  }

  if (on)
  {
    reg.LP_EN = 1;
  }
  else
  {
    reg.LP_EN = 0;
  }

  retval = ICM_20948_execute_w(pdev, REG_PWR_MGMT_1, (uint8_t *)&reg, sizeof(ICM_20948_PWR_MGMT_1_t));
  if (retval != ICM_20948_Stat_Ok)
  {
    return retval;
  }
  return retval;
}

ICM_20948_Status_e ICM_20948_set_clock_source(ICM_20948_Device_t *pdev, ICM_20948_PWR_MGMT_1_CLKSEL_e source)
{
  ICM_20948_Status_e retval = ICM_20948_Stat_Ok;
  ICM_20948_PWR_MGMT_1_t reg;

  ICM_20948_set_bank(pdev, 0); // Must be in the right bank

  retval = ICM_20948_execute_r(pdev, REG_PWR_MGMT_1, (uint8_t *)&reg, sizeof(ICM_20948_PWR_MGMT_1_t));
  if (retval != ICM_20948_Stat_Ok)
  {
    return retval;
  }

  reg.CLKSEL = source;

  retval = ICM_20948_execute_w(pdev, REG_PWR_MGMT_1, (uint8_t *)&reg, sizeof(ICM_20948_PWR_MGMT_1_t));
  if (retval != ICM_20948_Stat_Ok)
  {
    return retval;
  }
  return retval;
}

ICM_20948_Status_e ICM_20948_set_sample_mode(ICM_20948_Device_t *pdev, ICM_20948_InternalSensorID_bm sensors, ICM_20948_LP_CONFIG_CYCLE_e mode)
{
  ICM_20948_Status_e retval = ICM_20948_Stat_Ok;
  ICM_20948_LP_CONFIG_t reg;

  if (!(sensors & (ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr | ICM_20948_Internal_Mst)))
  {
    return ICM_20948_Stat_SensorNotSupported;
  }

  retval = ICM_20948_set_bank(pdev, 0); // Must be in the right bank
  if (retval != ICM_20948_Stat_Ok)
  {
    return retval;
  }
  retval = ICM_20948_execute_r(pdev, REG_LP_CONFIG, (uint8_t *)&reg, sizeof(ICM_20948_LP_CONFIG_t));
  if (retval != ICM_20948_Stat_Ok)
  {
    return retval;
  }

  if (sensors & ICM_20948_Internal_Acc)
  {
    reg.ACCEL_CYCLE = mode;
  } // Set all desired sensors to this setting
  if (sensors & ICM_20948_Internal_Gyr)
  {
    reg.GYRO_CYCLE = mode;
  }
  if (sensors & ICM_20948_Internal_Mst)
  {
    reg.I2C_MST_CYCLE = mode;
  }

  retval = ICM_20948_execute_w(pdev, REG_LP_CONFIG, (uint8_t *)&reg, sizeof(ICM_20948_LP_CONFIG_t));
  if (retval != ICM_20948_Stat_Ok)
  {
    return retval;
  }
  return retval;
}

ICM_20948_Status_e ICM_20948_set_full_scale(ICM_20948_Device_t *pdev, ICM_20948_InternalSensorID_bm sensors, ICM_20948_fss_t fss)
{
  ICM_20948_Status_e retval = ICM_20948_Stat_Ok;

  if (!(sensors & (ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr)))
  {
    return ICM_20948_Stat_SensorNotSupported;
  }

  if (sensors & ICM_20948_Internal_Acc)
  {
    ICM_20948_ACCEL_CONFIG_t reg;
    retval |= ICM_20948_set_bank(pdev, 2); // Must be in the right bank
    retval |= ICM_20948_execute_r(pdev, REG_ACCEL_CONFIG, (uint8_t *)&reg, sizeof(ICM_20948_ACCEL_CONFIG_t));
    reg.ACCEL_FS_SEL = fss.a;
    retval |= ICM_20948_execute_w(pdev, REG_ACCEL_CONFIG, (uint8_t *)&reg, sizeof(ICM_20948_ACCEL_CONFIG_t));
  }
  if (sensors & ICM_20948_Internal_Gyr)
  {
    ICM_20948_GYRO_CONFIG_1_t reg;
    retval |= ICM_20948_set_bank(pdev, 2); // Must be in the right bank
    retval |= ICM_20948_execute_r(pdev, REG_GYRO_CONFIG_1, (uint8_t *)&reg, sizeof(ICM_20948_GYRO_CONFIG_1_t));
    reg.GYRO_FS_SEL = fss.g;
    retval |= ICM_20948_execute_w(pdev, REG_GYRO_CONFIG_1, (uint8_t *)&reg, sizeof(ICM_20948_GYRO_CONFIG_1_t));
  }
  return retval;
}

ICM_20948_Status_e ICM_20948_set_dlpf_cfg(ICM_20948_Device_t *pdev, ICM_20948_InternalSensorID_bm sensors, ICM_20948_dlpcfg_t cfg)
{
  ICM_20948_Status_e retval = ICM_20948_Stat_Ok;

  if (!(sensors & (ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr)))
  {
    return ICM_20948_Stat_SensorNotSupported;
  }

  if (sensors & ICM_20948_Internal_Acc)
  {
    ICM_20948_ACCEL_CONFIG_t reg;
    retval |= ICM_20948_set_bank(pdev, 2); // Must be in the right bank
    retval |= ICM_20948_execute_r(pdev, REG_ACCEL_CONFIG, (uint8_t *)&reg, sizeof(ICM_20948_ACCEL_CONFIG_t));
    reg.ACCEL_DLPFCFG = cfg.a;
    retval |= ICM_20948_execute_w(pdev, REG_ACCEL_CONFIG, (uint8_t *)&reg, sizeof(ICM_20948_ACCEL_CONFIG_t));
  }
  if (sensors & ICM_20948_Internal_Gyr)
  {
    ICM_20948_GYRO_CONFIG_1_t reg;
    retval |= ICM_20948_set_bank(pdev, 2); // Must be in the right bank
    retval |= ICM_20948_execute_r(pdev, REG_GYRO_CONFIG_1, (uint8_t *)&reg, sizeof(ICM_20948_GYRO_CONFIG_1_t));
    reg.GYRO_DLPFCFG = cfg.g;
    retval |= ICM_20948_execute_w(pdev, REG_GYRO_CONFIG_1, (uint8_t *)&reg, sizeof(ICM_20948_GYRO_CONFIG_1_t));
  }
  return retval;
}

ICM_20948_Status_e ICM_20948_enable_dlpf(ICM_20948_Device_t *pdev, ICM_20948_InternalSensorID_bm sensors, bool enable)
{
  ICM_20948_Status_e retval = ICM_20948_Stat_Ok;

  if (!(sensors & (ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr)))
  {
    return ICM_20948_Stat_SensorNotSupported;
  }

  if (sensors & ICM_20948_Internal_Acc)
  {
    ICM_20948_ACCEL_CONFIG_t reg;
    retval |= ICM_20948_set_bank(pdev, 2); // Must be in the right bank
    retval |= ICM_20948_execute_r(pdev, REG_ACCEL_CONFIG, (uint8_t *)&reg, sizeof(ICM_20948_ACCEL_CONFIG_t));
    if (enable)
    {
      reg.ACCEL_FCHOICE = 1;
    }
    else
    {
      reg.ACCEL_FCHOICE = 0;
    }
    retval |= ICM_20948_execute_w(pdev, REG_ACCEL_CONFIG, (uint8_t *)&reg, sizeof(ICM_20948_ACCEL_CONFIG_t));
  }
  if (sensors & ICM_20948_Internal_Gyr)
  {
    ICM_20948_GYRO_CONFIG_1_t reg;
    retval |= ICM_20948_set_bank(pdev, 2); // Must be in the right bank
    retval |= ICM_20948_execute_r(pdev, REG_GYRO_CONFIG_1, (uint8_t *)&reg, sizeof(ICM_20948_GYRO_CONFIG_1_t));
    if (enable)
    {
      reg.GYRO_FCHOICE = 1;
    }
    else
    {
      reg.GYRO_FCHOICE = 0;
    }
    retval |= ICM_20948_execute_w(pdev, REG_GYRO_CONFIG_1, (uint8_t *)&reg, sizeof(ICM_20948_GYRO_CONFIG_1_t));
  }
  return retval;
}


ICM_20948_Status_e ICM_20948_set_sample_rate(ICM_20948_Device_t *pdev, ICM_20948_InternalSensorID_bm sensors, ICM_20948_smplrt_t smplrt)
{
  ICM_20948_Status_e retval = ICM_20948_Stat_Ok;

  if (!(sensors & (ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr)))
  {
    return ICM_20948_Stat_SensorNotSupported;
  }

  if (sensors & ICM_20948_Internal_Acc)
  {
    retval |= ICM_20948_set_bank(pdev, 2); // Must be in the right bank
    uint8_t div1 = (smplrt.a << 8);
    uint8_t div2 = (smplrt.a & 0xFF);
    retval |= ICM_20948_execute_w(pdev, REG_ACCEL_SMPLRT_DIV_1, &div1, 1);
    retval |= ICM_20948_execute_w(pdev, REG_ACCEL_SMPLRT_DIV_2, &div2, 1);
  }
  if (sensors & ICM_20948_Internal_Gyr)
  {
    retval |= ICM_20948_set_bank(pdev, 2); // Must be in the right bank
    uint8_t div = (smplrt.g);
    retval |= ICM_20948_execute_w(pdev, REG_GYRO_SMPLRT_DIV, &div, 1);
  }
  return retval;
}

static const uint16_t icm20948_gyro_sensitivity_x10[] = {
	1310, 655, 328, 164
};

/* see "Accelerometer Measurements" section from register map description */
static void icm20948_convert_accel(struct sensor_value *val,
				   int16_t raw_val,
				   uint16_t sensitivity_shift)
{
	int64_t conv_val;

	conv_val = ((int64_t)raw_val * SENSOR_G) >> sensitivity_shift;
	val->val1 = conv_val / 1000000;
	val->val2 = conv_val % 1000000;
}

/* see "Gyroscope Measurements" section from register map description */
static void icm20948_convert_gyro(struct sensor_value *val,
				  int16_t raw_val,
				  uint16_t sensitivity_x10)
{
	int64_t conv_val;

	conv_val = ((int64_t)raw_val * SENSOR_PI * 10) /
		   (sensitivity_x10 * 180U);
	val->val1 = conv_val / 1000000;
	val->val2 = conv_val % 1000000;
}

/* see "Temperature Measurement" section from register map description */
static inline void icm20948_convert_temp(struct sensor_value *val,
					 int16_t raw_val)
{
	val->val1 = (((int64_t)raw_val * 100) / 207) + 25;
	val->val2 = ((((int64_t)raw_val * 100) % 207) * 1000000) / 207;

	if (val->val2 < 0) {
		val->val1--;
		val->val2 += 1000000;
	} else if (val->val2 >= 1000000) {
		val->val1++;
		val->val2 -= 1000000;
	}
}

static int icm20948_channel_get(const struct device *dev,
				enum sensor_channel chan,
				struct sensor_value *val)
{
	const struct icm20948_data *drv_data = dev->data;

	switch (chan) {
	case SENSOR_CHAN_ACCEL_XYZ:
		icm20948_convert_accel(val, drv_data->accel_x,
				       drv_data->accel_sensitivity_shift);
		icm20948_convert_accel(val + 1, drv_data->accel_y,
				       drv_data->accel_sensitivity_shift);
		icm20948_convert_accel(val + 2, drv_data->accel_z,
				       drv_data->accel_sensitivity_shift);
		break;
	case SENSOR_CHAN_ACCEL_X:
		icm20948_convert_accel(val, drv_data->accel_x,
				       drv_data->accel_sensitivity_shift);
		break;
	case SENSOR_CHAN_ACCEL_Y:
		icm20948_convert_accel(val, drv_data->accel_y,
				       drv_data->accel_sensitivity_shift);
		break;
	case SENSOR_CHAN_ACCEL_Z:
		icm20948_convert_accel(val, drv_data->accel_z,
				       drv_data->accel_sensitivity_shift);
		break;
	case SENSOR_CHAN_GYRO_XYZ:
		icm20948_convert_gyro(val, drv_data->gyro_x,
				      drv_data->gyro_sensitivity_x10);
		icm20948_convert_gyro(val + 1, drv_data->gyro_y,
				      drv_data->gyro_sensitivity_x10);
		icm20948_convert_gyro(val + 2, drv_data->gyro_z,
				      drv_data->gyro_sensitivity_x10);
		break;
	case SENSOR_CHAN_GYRO_X:
		icm20948_convert_gyro(val, drv_data->gyro_x,
				      drv_data->gyro_sensitivity_x10);
		break;
	case SENSOR_CHAN_GYRO_Y:
		icm20948_convert_gyro(val, drv_data->gyro_y,
				      drv_data->gyro_sensitivity_x10);
		break;
	case SENSOR_CHAN_GYRO_Z:
		icm20948_convert_gyro(val, drv_data->gyro_z,
				      drv_data->gyro_sensitivity_x10);
		break;
	case SENSOR_CHAN_DIE_TEMP:
		icm20948_convert_temp(val, drv_data->temp);
		break;
	default:
		return -ENOTSUP;
	}

	return 0;
}


static int icm20948_sample_fetch(const struct device *dev,
				 enum sensor_channel chan)
{
    // ICM_20948_get_agmt
    struct device *pdev = (struct device *)dev;
    struct icm20948_data *drv_data = dev->data;

    ICM_20948_Status_e retval = ICM_20948_Stat_Ok;
    const uint8_t numbytes = 14 + 9; //Read Accel, gyro, temp, and 9 bytes of mag
    uint8_t buff[numbytes];

    // Get readings
    retval |= ICM_20948_set_bank(pdev, 0);
    retval |= ICM_20948_execute_r(pdev, (uint8_t)REG_ACCEL_XOUT_H, buff, numbytes);

    drv_data->accel_x = ((buff[0] << 8) | (buff[1] & 0xFF));
    drv_data->accel_y = ((buff[2] << 8) | (buff[3] & 0xFF));
    drv_data->accel_z = ((buff[4] << 8) | (buff[5] & 0xFF));

    drv_data->gyro_x = ((buff[6] << 8) | (buff[7] & 0xFF));
    drv_data->gyro_y = ((buff[8] << 8) | (buff[9] & 0xFF));
    drv_data->gyro_z = ((buff[10] << 8) | (buff[11] & 0xFF));

    drv_data->temp = ((buff[12] << 8) | (buff[13] & 0xFF));

    // TODO: magnetometer!

    // Get settings to be able to compute scaled values
    retval |= ICM_20948_set_bank(pdev, 2);
    ICM_20948_ACCEL_CONFIG_t acfg;
    retval |= ICM_20948_execute_r(pdev, (uint8_t)REG_ACCEL_CONFIG, (uint8_t *)&acfg, 1 * sizeof(acfg));
    drv_data->fss.a = acfg.ACCEL_FS_SEL; // Worth noting that without explicitly setting the FS range of the accelerometer it was showing the register value for +/- 2g but the reported values were actually scaled to the +/- 16g range
                                    // Wait a minute... now it seems like this problem actually comes from the digital low-pass filter. When enabled the value is 1/8 what it should be...
    retval |= ICM_20948_set_bank(pdev, 2);
    ICM_20948_GYRO_CONFIG_1_t gcfg1;
    retval |= ICM_20948_execute_r(pdev, (uint8_t)REG_GYRO_CONFIG_1, (uint8_t *)&gcfg1, 1 * sizeof(gcfg1));
    drv_data->fss.g = gcfg1.GYRO_FS_SEL;
    ICM_20948_ACCEL_CONFIG_2_t acfg2;
    retval |= ICM_20948_execute_r(pdev, (uint8_t)REG_ACCEL_CONFIG_2, (uint8_t *)&acfg2, 1 * sizeof(acfg2));

    return retval;

	/* int result = 0; */
	/* uint16_t fifo_count = 0; */
	/* struct icm20948_data *drv_data = dev->data; */

	/* /\* Read INT_STATUS (0x45) and FIFO_COUNTH(0x46), FIFO_COUNTL(0x47) *\/ */
	/* result = inv_spi_read(REG_INT_STATUS, drv_data->fifo_data, 3); */

	/* if (drv_data->fifo_data[0] & BIT_INT_STATUS_DRDY) { */
	/* 	fifo_count = (drv_data->fifo_data[1] << 8) */
	/* 		+ (drv_data->fifo_data[2]); */
	/* 	result = inv_spi_read(REG_FIFO_DATA, drv_data->fifo_data, */
	/* 			      fifo_count); */

	/* 	/\* FIFO Data structure */
	/* 	 * Packet 1 : FIFO Header(1), AccelX(2), AccelY(2), */
	/* 	 *            AccelZ(2), Temperature(1) */
	/* 	 * Packet 2 : FIFO Header(1), GyroX(2), GyroY(2), */
	/* 	 *            GyroZ(2), Temperature(1) */
	/* 	 * Packet 3 : FIFO Header(1), AccelX(2), AccelY(2), AccelZ(2), */
	/* 	 *            GyroX(2), GyroY(2), GyroZ(2), Temperature(1) */
	/* 	 *\/ */
	/* 	if (drv_data->fifo_data[0] & BIT_FIFO_HEAD_ACCEL) { */
	/* 		/\* Check empty values *\/ */
	/* 		if (!(drv_data->fifo_data[1] == FIFO_ACCEL0_RESET_VALUE */
	/* 		      && drv_data->fifo_data[2] == */
	/* 		      FIFO_ACCEL1_RESET_VALUE)) { */
	/* 			drv_data->accel_x = */
	/* 				(drv_data->fifo_data[1] << 8) */
	/* 				+ (drv_data->fifo_data[2]); */
	/* 			drv_data->accel_y = */
	/* 				(drv_data->fifo_data[3] << 8) */
	/* 				+ (drv_data->fifo_data[4]); */
	/* 			drv_data->accel_z = */
	/* 				(drv_data->fifo_data[5] << 8) */
	/* 				+ (drv_data->fifo_data[6]); */
	/* 		} */
	/* 		if (!(drv_data->fifo_data[0] & BIT_FIFO_HEAD_GYRO)) { */
	/* 			drv_data->temp = */
	/* 				(int16_t)(drv_data->fifo_data[7]); */
	/* 		} else { */
	/* 			if (!(drv_data->fifo_data[7] == */
	/* 			      FIFO_GYRO0_RESET_VALUE && */
	/* 			      drv_data->fifo_data[8] == */
	/* 			      FIFO_GYRO1_RESET_VALUE)) { */
	/* 				drv_data->gyro_x = */
	/* 					(drv_data->fifo_data[7] << 8) */
	/* 					+ (drv_data->fifo_data[8]); */
	/* 				drv_data->gyro_y = */
	/* 					(drv_data->fifo_data[9] << 8) */
	/* 					+ (drv_data->fifo_data[10]); */
	/* 				drv_data->gyro_z = */
	/* 					(drv_data->fifo_data[11] << 8) */
	/* 					+ (drv_data->fifo_data[12]); */
	/* 			} */
	/* 			drv_data->temp = */
	/* 				(int16_t)(drv_data->fifo_data[13]); */
	/* 		} */
	/* 	} else { */
	/* 		if (drv_data->fifo_data[0] & BIT_FIFO_HEAD_GYRO) { */
	/* 			if (!(drv_data->fifo_data[1] == */
	/* 			      FIFO_GYRO0_RESET_VALUE && */
	/* 			      drv_data->fifo_data[2] == */
	/* 			      FIFO_GYRO1_RESET_VALUE)) { */
	/* 				drv_data->gyro_x = */
	/* 					(drv_data->fifo_data[1] << 8) */
	/* 					+ (drv_data->fifo_data[2]); */
	/* 				drv_data->gyro_y = */
	/* 					(drv_data->fifo_data[3] << 8) */
	/* 					+ (drv_data->fifo_data[4]); */
	/* 				drv_data->gyro_z = */
	/* 					(drv_data->fifo_data[5] << 8) */
	/* 					+ (drv_data->fifo_data[6]); */
	/* 			} */
	/* 			drv_data->temp = */
	/* 				(int16_t)(drv_data->fifo_data[7]); */
	/* 		} */
	/* 	} */
	/* } */

	return 0;
}

static int icm20948_attr_set(const struct device *dev,
			     enum sensor_channel chan,
			     enum sensor_attribute attr,
			     const struct sensor_value *val)
{
//	struct icm20948_data *drv_data = dev->data;

	__ASSERT_NO_MSG(val != NULL);

	/* switch (chan) { */
	/* case SENSOR_CHAN_ACCEL_X: */
	/* case SENSOR_CHAN_ACCEL_Y: */
	/* case SENSOR_CHAN_ACCEL_Z: */
	/* case SENSOR_CHAN_ACCEL_XYZ: */
	/* 	if (attr == SENSOR_ATTR_SAMPLING_FREQUENCY) { */
	/* 		if (val->val1 > 8000 || val->val1 < 1) { */
	/* 			LOG_ERR("Incorrect sampling value"); */
	/* 			return -EINVAL; */
	/* 		} else { */
	/* 			drv_data->accel_hz = val->val1; */
	/* 		} */
	/* 	} else if (attr == SENSOR_ATTR_FULL_SCALE) { */
	/* 		if (val->val1 < ACCEL_FS_16G || */
	/* 		    val->val1 > ACCEL_FS_2G) { */
	/* 			LOG_ERR("Incorrect fullscale value"); */
	/* 			return -EINVAL; */
	/* 		} else { */
	/* 			drv_data->accel_sf = val->val1; */
	/* 		} */
	/* 	} else { */
	/* 		LOG_ERR("Not supported ATTR"); */
	/* 		return -ENOTSUP; */
	/* 	} */

	/* 	break; */
	/* case SENSOR_CHAN_GYRO_X: */
	/* case SENSOR_CHAN_GYRO_Y: */
	/* case SENSOR_CHAN_GYRO_Z: */
	/* case SENSOR_CHAN_GYRO_XYZ: */
	/* 	if (attr == SENSOR_ATTR_SAMPLING_FREQUENCY) { */
	/* 		if (val->val1 > 8000 || val->val1 < 12) { */
	/* 			LOG_ERR("Incorrect sampling value"); */
	/* 			return -EINVAL; */
	/* 		} else { */
	/* 			drv_data->gyro_hz = val->val1; */
	/* 		} */
	/* 	} else if (attr == SENSOR_ATTR_FULL_SCALE) { */
	/* 		if (val->val1 < GYRO_FS_2000DPS || */
	/* 		    val->val1 > GYRO_FS_15DPS) { */
	/* 			LOG_ERR("Incorrect fullscale value"); */
	/* 			return -EINVAL; */
	/* 		} else { */
	/* 			drv_data->gyro_sf = val->val1; */
	/* 		} */
	/* 	} else { */
	/* 		LOG_ERR("Not supported ATTR"); */
	/* 		return -EINVAL; */
	/* 	} */
	/* 	break; */
	/* default: */
	/* 	LOG_ERR("Not support"); */
	/* 	return -EINVAL; */
	/* } */

	return 0;
}

static int icm20948_attr_get(const struct device *dev,
			     enum sensor_channel chan,
			     enum sensor_attribute attr,
			     struct sensor_value *val)
{
//	const struct icm20948_data *drv_data = dev->data;

	__ASSERT_NO_MSG(val != NULL);

	/* switch (chan) { */
	/* case SENSOR_CHAN_ACCEL_X: */
	/* case SENSOR_CHAN_ACCEL_Y: */
	/* case SENSOR_CHAN_ACCEL_Z: */
	/* case SENSOR_CHAN_ACCEL_XYZ: */
	/* 	if (attr == SENSOR_ATTR_SAMPLING_FREQUENCY) { */
	/* 		val->val1 = drv_data->accel_hz; */
	/* 	} else if (attr == SENSOR_ATTR_FULL_SCALE) { */
	/* 		val->val1 = drv_data->accel_sf; */
	/* 	} else { */
	/* 		LOG_ERR("Not supported ATTR"); */
	/* 		return -EINVAL; */
	/* 	} */

	/* 	break; */
	/* case SENSOR_CHAN_GYRO_X: */
	/* case SENSOR_CHAN_GYRO_Y: */
	/* case SENSOR_CHAN_GYRO_Z: */
	/* case SENSOR_CHAN_GYRO_XYZ: */
	/* 	if (attr == SENSOR_ATTR_SAMPLING_FREQUENCY) { */
	/* 		val->val1 = drv_data->gyro_hz; */
	/* 	} else if (attr == SENSOR_ATTR_FULL_SCALE) { */
	/* 		val->val1 = drv_data->gyro_sf; */
	/* 	} else { */
	/* 		LOG_ERR("Not supported ATTR"); */
	/* 		return -EINVAL; */
	/* 	} */

	/* 	break; */

	/* default: */
	/* 	LOG_ERR("Not support"); */
	/* 	return -EINVAL; */
	/* } */

	return 0;
}

static int icm20948_data_init(struct icm20948_data *data,
			      const struct icm20948_config *cfg)
{
	data->accel_x = 0;
	data->accel_y = 0;
	data->accel_z = 0;
	/* data->temp = 0; */
	data->gyro_x = 0;
	data->gyro_y = 0;
	data->gyro_z = 0;
	/* data->accel_hz = cfg->accel_hz; */
	/* data->gyro_hz = cfg->gyro_hz; */

	/* data->accel_sf = cfg->accel_fs; */
	/* data->gyro_sf = cfg->gyro_fs; */

	/* data->tap_en = false; */
	data->sensor_started = false;

	return 0;
}


static int icm20948_init(const struct device *dev)
{
	struct icm20948_data *drv_data = dev->data;
	const struct icm20948_config *cfg = dev->config;

        LOG_ERR("icm20948_init");

	drv_data->spi = device_get_binding(cfg->spi_label);
	if (!drv_data->spi) {
		LOG_ERR("SPI device not exist");
		return -ENODEV;
	}

	drv_data->spi_cs.gpio_dev = device_get_binding(cfg->gpio_label);

	if (!drv_data->spi_cs.gpio_dev) {
		LOG_ERR("GPIO device not exist");
		return -ENODEV;
	}

	drv_data->spi_cs.gpio_pin = cfg->gpio_pin;
	drv_data->spi_cs.gpio_dt_flags = cfg->gpio_dt_flags;
	drv_data->spi_cs.delay = 0U;

	drv_data->spi_cfg.frequency = cfg->frequency;
	drv_data->spi_cfg.slave = cfg->slave;
	drv_data->spi_cfg.operation = (SPI_OP_MODE_MASTER | SPI_MODE_CPOL |
			SPI_MODE_CPHA | SPI_WORD_SET(8) | SPI_LINES_SINGLE |
			SPI_TRANSFER_MSB);
	drv_data->spi_cfg.cs = &drv_data->spi_cs;

	icm20948_spi_init(drv_data->spi, &drv_data->spi_cfg);
	icm20948_data_init(drv_data, cfg);
	icm20948_sensor_init(dev);

 	drv_data->accel_sensitivity_shift = 14 - 3;
 	drv_data->gyro_sensitivity_x10 = icm20948_gyro_sensitivity_x10[3];

/* #ifdef CONFIG_ICM20948_TRIGGER */
/* 	if (icm20948_init_interrupt(dev) < 0) { */
/* 		LOG_ERR("Failed to initialize interrupts."); */
/* 		return -EIO; */
/* 	} */
/* #endif */

	LOG_ERR("Initialize interrupt done"); // used to be LOG_DBG

	return 0;
}

static const struct sensor_driver_api icm20948_driver_api = {
/* #ifdef CONFIG_ICM20948_TRIGGER */
/* 	.trigger_set = icm20948_trigger_set, */
/* #endif */
	.sample_fetch = icm20948_sample_fetch,
	.channel_get = icm20948_channel_get,
	.attr_set = icm20948_attr_set,
	.attr_get = icm20948_attr_get,
};

#define ICM20948_DEFINE_CONFIG(index)					\
	static const struct icm20948_config icm20948_cfg_##index = {	\
		.spi_label = DT_INST_BUS_LABEL(index),			\
		.spi_addr = DT_INST_REG_ADDR(index),			\
		.frequency = DT_INST_PROP(index, spi_max_frequency),	\
		.slave = DT_INST_REG_ADDR(index),			\
		.int_label = DT_INST_GPIO_LABEL(index, int_gpios),	\
		.int_pin =  DT_INST_GPIO_PIN(index, int_gpios),		\
		.int_flags = DT_INST_GPIO_FLAGS(index, int_gpios),	\
		.gpio_label = DT_INST_SPI_DEV_CS_GPIOS_LABEL(index),	\
		.gpio_pin = DT_INST_SPI_DEV_CS_GPIOS_PIN(index),	\
		.gpio_dt_flags = DT_INST_SPI_DEV_CS_GPIOS_FLAGS(index),	\
                /*		.accel_hz = DT_INST_PROP(index, accel_hz),*/ \
		/* .gyro_hz = DT_INST_PROP(index, gyro_hz),	*/	\
		/* .accel_fs = DT_ENUM_IDX(DT_DRV_INST(index), accel_fs), */ \
		/* .gyro_fs = DT_ENUM_IDX(DT_DRV_INST(index), gyro_fs), */ \
	}

#define ICM20948_INIT(index)						\
	ICM20948_DEFINE_CONFIG(index);					\
	static struct icm20948_data icm20948_driver_##index;		\
	DEVICE_DT_INST_DEFINE(index, icm20948_init,			\
			    NULL,					\
			    &icm20948_driver_##index,			\
			    &icm20948_cfg_##index, POST_KERNEL,		\
			    CONFIG_SENSOR_INIT_PRIORITY,		\
			    &icm20948_driver_api);

DT_INST_FOREACH_STATUS_OKAY(ICM20948_INIT)
