The purpose of this repository is understanding,

structure and pointer
i2c adapter and algorithm in kernel
get/set_drv_data api
Test adxl driver and try to print xyz data in kernel.Later need to see how to send data to user space.
later use mutex at appropriate points in the driver
try to set tap functionality using interrupts.

1. I will first simplify the adxl34x driver to better understand the philosophy of how a production level driver is written
Kernel requirements,
I2C requirements,
device tree requirements,
userspace requirements,
How this subsystem works,
add 16x2 LCD to show x, y, z values.


2. Below will be my notable changes or learnings that I have done.

A. 
device tree overlay is dtbo
device tree blob is dtb

to load device tree overlay while boot up, we must use uboot overlay as kernel overlay is deprecated and not in use.
https://groups.google.com/forum/#!msg/beagleboard/1ahosuHBUTo/ya-8Od6jAAAJ
So change uEnv.txt to

enable_uboot_overlays=1
uboot_overlay_addr0=/lib/firmware/BB-I2C2-ADXL-00A0.dtbo

here I have changed 
compatible = "adi, adxl345"
in BB-I2C2-ADXL-00A0.dts file
