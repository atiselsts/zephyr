#include <zephyr.h>
#include <device.h>
#include <drivers/sensor.h>
#include <stdio.h>

static const char *now_str(void)
{
	static char buf[16]; /* ...HH:MM:SS.MMM */
	uint32_t now = k_uptime_get_32();
	unsigned int ms = now % MSEC_PER_SEC;
	unsigned int s;
	unsigned int min;
	unsigned int h;

	now /= MSEC_PER_SEC;
	s = now % 60U;
	now /= 60U;
	min = now % 60U;
	now /= 60U;
	h = now;

	snprintf(buf, sizeof(buf), "%u:%02u:%02u.%03u",
		 h, min, s, ms);
	return buf;
}

static int process_icm20948(const struct device *dev)
{
	struct sensor_value temperature;
	struct sensor_value accel[3];
	int rc = sensor_sample_fetch(dev);

	if (rc == 0) {
		rc = sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ,
					accel);
	}
	/* if (rc == 0) { */
	/* 	rc = sensor_channel_get(dev, SENSOR_CHAN_GYRO_XYZ, */
	/* 				gyro); */
	/* } */
	/* if (rc == 0) { */
	/* 	rc = sensor_channel_get(dev, SENSOR_CHAN_DIE_TEMP, */
	/* 				&temperature); */
	/* } */
	if (rc == 0) {
		printf("[%s]: \n"
		       "  accel % f % f % f m/s/s\n",
		       now_str(),
		       sensor_value_to_double(&accel[0]),
		       sensor_value_to_double(&accel[1]),
		       sensor_value_to_double(&accel[2]));
	} else {
		printf("sample fetch/get failed: %d\n", rc);
	}

	return rc;
}

static struct sensor_trigger data_trigger;

static void handle_icm42605_drdy(const struct device *dev,
				 struct sensor_trigger *trig)
{
	int rc = process_icm42605(dev);

	if (rc != 0) {
		printf("cancelling trigger due to failure: %d\n", rc);
		(void)sensor_trigger_set(dev, trig, NULL);
		return;
	}
}

/* static void handle_icm42605_tap(const struct device *dev, */
/* 				 struct sensor_trigger *trig) */
/* { */
/* 	printf("Tap Detected!\n"); */
/* } */

/* static void handle_icm42605_double_tap(const struct device *dev, */
/* 				 struct sensor_trigger *trig) */
/* { */
/* 	printf("Double Tap detected!\n"); */
/* } */

void main(void)
{
	const char *const label = DT_LABEL(DT_INST(0, invensense_icm20948));
	const struct device *icm20948 = device_get_binding(label);

	if (!icm20948) {
		printf("Failed to find sensor %s\n", label);
		return;
	}

	/* tap_trigger = (struct sensor_trigger) { */
	/* 	.type = SENSOR_TRIG_TAP, */
	/* 	.chan = SENSOR_CHAN_ALL, */
	/* }; */

	/* if (sensor_trigger_set(icm42605, &tap_trigger, */
	/* 		       handle_icm42605_tap) < 0) { */
	/* 	printf("Cannot configure tap trigger!!!\n"); */
	/* 	return; */
	/* } */

	/* double_tap_trigger = (struct sensor_trigger) { */
	/* 	.type = SENSOR_TRIG_DOUBLE_TAP, */
	/* 	.chan = SENSOR_CHAN_ALL, */
	/* }; */

	/* if (sensor_trigger_set(icm42605, &double_tap_trigger, */
	/* 		       handle_icm42605_double_tap) < 0) { */
	/* 	printf("Cannot configure double tap trigger!!!\n"); */
	/* 	return; */
	/* } */

	data_trigger = (struct sensor_trigger) {
		.type = SENSOR_TRIG_DATA_READY,
		.chan = SENSOR_CHAN_ALL,
	};

	if (sensor_trigger_set(icm42605, &data_trigger,
			       handle_icm42605_drdy) < 0) {
		printf("Cannot configure data trigger!!!\n");
		return;
	}

	printf("Configured for sensor sampling.\n");
}
