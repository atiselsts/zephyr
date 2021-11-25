.. _icm20948:

ICM 20948 IMU sensor demonstrator 
##########################################

Description
***********

This sample application periodically (10 Hz) measures the sensor
temperature, acceleration, and angular velocity, tap, double tap
displaying the values on the console along with a timestamp since
startup.

Wiring
*******

This sample uses an external breakout for the sensor.  A devicetree
overlay must be provided to identify the SPI bus and GPIO used to
control the sensor.

Building and Running
********************

After providing a devicetree overlay that specifies the sensor location,
build this sample app using:

.. zephyr-app-commands::
   :zephyr-app: samples/sensor/icm20948
   :board: nrf5340dk_nrf5340_cpuapp
   :goals: build flash

Sample Output
=============

.. code-block:: console

   TODO!

<repeats endlessly>
