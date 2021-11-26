#include <zephyr.h>
#include <device.h>
#include <drivers/sensor.h>
#include <stdio.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

/* static const char *now_str(void) */
/* { */
/* 	static char buf[16]; /\* ...HH:MM:SS.MMM *\/ */
/* 	uint32_t now = k_uptime_get_32(); */
/* 	unsigned int ms = now % MSEC_PER_SEC; */
/* 	unsigned int s; */
/* 	unsigned int min; */
/* 	unsigned int h; */

/* 	now /= MSEC_PER_SEC; */
/* 	s = now % 60U; */
/* 	now /= 60U; */
/* 	min = now % 60U; */
/* 	now /= 60U; */
/* 	h = now; */

/* 	snprintf(buf, sizeof(buf), "%u:%02u:%02u.%03u", */
/* 		 h, min, s, ms); */
/* 	return buf; */
/* } */

/* static int process_icm20948(const struct device *dev) */
/* { */
/* //	struct sensor_value temperature; */
/* 	struct sensor_value accel[3]; */
/* 	int rc = sensor_sample_fetch(dev); */

/* 	if (rc == 0) { */
/* 		rc = sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, */
/* 					accel); */
/* 	} */
/* 	/\* if (rc == 0) { *\/ */
/* 	/\* 	rc = sensor_channel_get(dev, SENSOR_CHAN_GYRO_XYZ, *\/ */
/* 	/\* 				gyro); *\/ */
/* 	/\* } *\/ */
/* 	/\* if (rc == 0) { *\/ */
/* 	/\* 	rc = sensor_channel_get(dev, SENSOR_CHAN_DIE_TEMP, *\/ */
/* 	/\* 				&temperature); *\/ */
/* 	/\* } *\/ */
/* 	if (rc == 0) { */
/* 		printf("[%s]: \n" */
/* 		       "  accel % f % f % f m/s/s\n", */
/* 		       now_str(), */
/* 		       sensor_value_to_double(&accel[0]), */
/* 		       sensor_value_to_double(&accel[1]), */
/* 		       sensor_value_to_double(&accel[2])); */
/* 	} else { */
/* 		printf("sample fetch/get failed: %d\n", rc); */
/* 	} */

/* 	return rc; */
/* } */

/* static struct sensor_trigger data_trigger; */

/* static void handle_icm20948_drdy(const struct device *dev, */
/* 				 struct sensor_trigger *trig) */
/* { */
/* 	int rc = process_icm20948(dev); */

/* 	if (rc != 0) { */
/* 		printf("cancelling trigger due to failure: %d\n", rc); */
/* 		(void)sensor_trigger_set(dev, trig, NULL); */
/* 		return; */
/* 	} */
/* } */

/* /\* static void handle_icm20948_tap(const struct device *dev, *\/ */
/* /\* 				 struct sensor_trigger *trig) *\/ */
/* /\* { *\/ */
/* /\* 	printf("Tap Detected!\n"); *\/ */
/* /\* } *\/ */

/* /\* static void handle_icm20948_double_tap(const struct device *dev, *\/ */
/* /\* 				 struct sensor_trigger *trig) *\/ */
/* /\* { *\/ */
/* /\* 	printf("Double Tap detected!\n"); *\/ */
/* /\* } *\/ */

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

	/* if (sensor_trigger_set(icm20948, &tap_trigger, */
	/* 		       handle_icm20948_tap) < 0) { */
	/* 	printf("Cannot configure tap trigger!!!\n"); */
	/* 	return; */
	/* } */

	/* double_tap_trigger = (struct sensor_trigger) { */
	/* 	.type = SENSOR_TRIG_DOUBLE_TAP, */
	/* 	.chan = SENSOR_CHAN_ALL, */
	/* }; */

	/* if (sensor_trigger_set(icm20948, &double_tap_trigger, */
	/* 		       handle_icm20948_double_tap) < 0) { */
	/* 	printf("Cannot configure double tap trigger!!!\n"); */
	/* 	return; */
	/* } */

	/* data_trigger = (struct sensor_trigger) { */
	/* 	.type = SENSOR_TRIG_DATA_READY, */
	/* 	.chan = SENSOR_CHAN_ALL, */
	/* }; */

	/* if (sensor_trigger_set(icm20948, &data_trigger, */
	/* 		       handle_icm20948_drdy) < 0) { */
	/* 	printf("Cannot configure data trigger!!!\n"); */
	/* 	return; */
	/* } */

        printf("Configured for sensor sampling.\n");

        int rc;
        while (1) {
            rc = icm_20948_data_ready(icm20948);
            if (rc != 0) {
                printf("not ready, rc=%d.\n", rc);
            } else {
                    struct sensor_value accel[3];
                    int rc = sensor_sample_fetch(icm20948);

                    if (rc == 0) {
                        rc = sensor_channel_get(icm20948, SENSOR_CHAN_ACCEL_XYZ, accel);
                        if (rc == 0) {
                            printf("  x=%d y=%d z=%d\n",
                                    accel[0].val1, accel[1].val1, accel[2].val1);
                        } else {
                            printf("sensor_channel_get, rc=%d.\n", rc);
                        }
                    } else {
                        printf("sensor_sample_fetch, rc=%d.\n", rc);
                    }
            }

            k_msleep(SLEEP_TIME_MS);
	}
}
