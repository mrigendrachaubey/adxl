#include <linux/input.h>	/* BUS_I2C */
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/types.h>
#include <linux/pm.h>	
#include <linux/slab.h>
#include <linux/input/adxl34x.h>
#include "adxl34x.h"


static const struct adxl34x_platform_data adxl34x_default_init = {
	.tap_threshold = 35,
	.tap_duration = 3,
	.tap_latency = 20,
	.tap_window = 20,
	.tap_axis_control = ADXL_TAP_X_EN | ADXL_TAP_Y_EN | ADXL_TAP_Z_EN,
	.act_axis_control = 0xFF,
	.activity_threshold = 6,
	.inactivity_threshold = 4,
	.inactivity_time = 3,
	.free_fall_threshold = 8,
	.free_fall_time = 0x20,
	.data_rate = 8,
	.data_range = ADXL_FULL_RES,

	.ev_type = EV_ABS,
	.ev_code_x = ABS_X,	/* EV_REL */
	.ev_code_y = ABS_Y,	/* EV_REL */
	.ev_code_z = ABS_Z,	/* EV_REL */

	.ev_code_tap = {BTN_TOUCH, BTN_TOUCH, BTN_TOUCH}, /* EV_KEY {x,y,z} */
	.power_mode = ADXL_AUTO_SLEEP | ADXL_LINK,
	.fifo_mode = ADXL_FIFO_STREAM,
	.watermark = 0,
};

static int adxl34x_smbus_read(struct device *dev, unsigned char reg)
{
	struct i2c_client *client = to_i2c_client(dev);

	return i2c_smbus_read_byte_data(client, reg);
}

static int adxl34x_smbus_write(struct device *dev,
			       unsigned char reg, unsigned char val)
{
	struct i2c_client *client = to_i2c_client(dev);

	return i2c_smbus_write_byte_data(client, reg, val);
}


static const struct adxl34x_bus_ops adxl34x_smbus_bops = {
	.bustype	= BUS_I2C,
	.write		= adxl34x_smbus_write,
	.read		= adxl34x_smbus_read,
};

static int adxl34x_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct adxl34x *ac;
	const struct adxl34x_platform_data *pdata;
	int error, err;

	/*
		check if i2c adapter supports smbus byte transactions
	*/
	error = i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE_DATA);
	if (!error) {
		dev_err(&client->dev, "SMBUS Byte Data not Supported\n");
		return -EIO;
	}

	/*
		allocating memory for struct adxl34x
	*/
	ac = kzalloc(sizeof(*ac), GFP_KERNEL);
	if (!ac) {
		err = -ENOMEM;
		goto err_free_mem;
	}

	/*
		pdata gets the pointer to the driver data	
	*/
	pdata = dev_get_platdata(&client->dev);
	if (!pdata) {
		pdata = &adxl34x_default_init;
	};

	/*
		putting data to struct adxl34x
	*/
	ac->pdata = *pdata;
	pdata = &ac->pdata;
	ac->dev = &client->dev;
	ac->bops = &adxl34x_smbus_bops;

	i2c_set_clientdata(client, ac);

err_free_mem:
	kfree(ac);

	return 0;
}
static int adxl34x_i2c_remove(struct i2c_client *client)
{
	struct adxl34x *ac = i2c_get_clientdata(client);

	return adxl34x_remove(ac);
}

static int __maybe_unused adxl34x_i2c_suspend(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct adxl34x *ac = i2c_get_clientdata(client);

	//adxl34x_suspend(ac);//not using it for a while.

	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id adxl34x_of_id[] = {
	/*
	 * The ADXL346 is backward-compatible with the ADXL345. Differences are
	 * handled by runtime detection of the device model, there's thus no
	 * need for listing the "adi,adxl346" compatible value explicitly.
	 */
	{ .compatible = "adi,adxl345", },
	/*
	 * Deprecated, DT nodes should use one or more of the device-specific
	 * compatible values "adi,adxl345" and "adi,adxl346".
	 */
	{ .compatible = "adi,adxl34x", },
	{ }
};

MODULE_DEVICE_TABLE(of, adxl34x_of_id);
#endif

static struct i2c_driver adxl34x_driver = {
	.driver = {
		.name = "adxl34x",
		//.pm = &adxl34x_i2c_pm,
		.of_match_table = of_match_ptr(adxl34x_of_id),
	},
	.probe    = adxl34x_i2c_probe,
	.remove   = adxl34x_i2c_remove,
	//.id_table = adxl34x_id,
};

module_i2c_driver(adxl34x_driver);

MODULE_AUTHOR("Michael Hennerich <hennerich@blackfin.uclinux.org>");
MODULE_DESCRIPTION("ADXL345/346 Three-Axis Digital Accelerometer I2C Bus Driver");
MODULE_LICENSE("GPL");
