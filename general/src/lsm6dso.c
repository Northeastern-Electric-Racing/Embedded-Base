/*
        LSM6DSOXTR IMU DRIVER Source File
        Link to part Datasheet for reference:
        https://www.st.com/resource/en/datasheet/lsm6dsox.pdf

        Author: Hamza Iqbal
*/

#include "lsm6dso.h"

#define REG_RESOLUTION 32768 /* Half the range of a 16 bit signed integer */

#define ACCEL_RANGE \
	4 /* The range of values in g's returned from accelerometer */
#define GYRO_RANGE 1000 /* The range of values from the gyro in dps */

lsm6dso_t imu;

static inline int lsm6dso_read_reg(uint8_t *data,
						 uint8_t reg)
{
	return imu.read_reg(data, reg, 1);
}

static inline int lsm6dso_read_mult_reg(
						      uint8_t *data,
						      uint8_t reg,
						      uint8_t length)
{
	return imu.read_reg(data, reg, length);
}

static inline int lsm6dso_write_reg(uint8_t *data,
						  uint8_t reg)
{
	return imu.write_reg(data, reg, 1);
}

static inline int lsm6dso_write_mult_reg(uint8_t *data,
						  uint8_t reg, uint8_t length)
{
	return imu.write_reg(data, reg, length);
}

static int16_t accel_data_convert(int16_t raw_accel) {
  int8_t msb, lsb;
  int16_t val;
  msb = (raw_accel & 0x00FF) << 8;
  lsb = (raw_accel & 0xFF00) >> 8;
  val = msb | lsb;

  return (int16_t)(((int32_t)val * ACCEL_RANGE * 1000) / REG_RESOLUTION);
}

static int16_t gyro_data_convert(int16_t gyro_accel) {
  int8_t msb, lsb;
  int16_t val;
  msb = (gyro_accel & 0x00FF) << 8;
  lsb = (gyro_accel & 0xFF00) >> 8;
  val = msb | lsb;
  return (int16_t)(((int32_t)val * GYRO_RANGE * 100) / REG_RESOLUTION);
}

static int lsm6dso_ping_imu()
{
	uint8_t reg_data;
	int status;

	status = lsm6dso_read_reg(&reg_data, LSM6DSO_REG_DEVICE_ID);
	if (status != 0)
		return status;

	if (reg_data != 0x6C)
		return -1;

	return 0;
}

int lsm6dso_set_accel_cfg(int8_t odr_sel,
					int8_t fs_sel, int8_t lp_f2_enable)
{
	uint8_t config =
		(((odr_sel << 4) | (fs_sel << 2) | (lp_f2_enable << 1)) << 1);
	imu.accel_config = config;

	return lsm6dso_write_reg(&imu.accel_config, LSM6DSO_REG_ACCEL_CTRL);
}

int lsm6dso_set_gyro_cfg(int8_t odr_sel,
				       int8_t fs_sel, int8_t fs_125)
{
	uint8_t config =
		(((odr_sel << 4) | (fs_sel << 2) | (fs_125 << 1)) << 1);
	imu.gyro_config = config;

	return lsm6dso_write_reg(&imu.gyro_config, LSM6DSO_REG_GYRO_CTRL);
}

int lsm6dso_init(I2C_Read read_func, I2C_Write write_func)
{
	int status;

	imu.accel_data[LSM6DSO_X_AXIS] = 0;
	imu.accel_data[LSM6DSO_Y_AXIS] = 0;
	imu.accel_data[LSM6DSO_Z_AXIS] = 0;

	imu.gyro_data[LSM6DSO_X_AXIS] = 0;
	imu.gyro_data[LSM6DSO_Y_AXIS] = 0;
	imu.gyro_data[LSM6DSO_Z_AXIS] = 0;
	
	imu.read_reg = read_func;
	imu.write_reg = write_func;

	/* Quick check to make sure I2C is working */
	status = lsm6dso_ping_imu();
	if (status != 0)
		return status;

	/*
		Configure IMU to default params
		Refer to datasheet pages 56-57
	*/
	status = lsm6dso_set_accel_cfg(0x08, 0x02, 0x01);
	if (status != 0)
		return status;

	status = lsm6dso_set_gyro_cfg(0x08, 0x02, 0x00);
	if (status != 0)
		return status;

	return 0;
}

int lsm6dso_read_accel()
{
	union {
		uint8_t buf[2];
		int16_t data;
	} accel_x_raw, accel_y_raw, accel_z_raw;
	int status;

	/* Getting raw data from registers */
	status = lsm6dso_read_mult_reg(accel_x_raw.buf,
				       LSM6DSO_REG_ACCEL_X_AXIS_L, 2);
	if (status != 0)
		return status;

	status = lsm6dso_read_mult_reg(accel_y_raw.buf,
				       LSM6DSO_REG_ACCEL_Y_AXIS_L, 2);
	if (status != 0)
		return status;

	status = lsm6dso_read_mult_reg(accel_z_raw.buf,
				       LSM6DSO_REG_ACCEL_Z_AXIS_L, 2);
	if (status != 0)
		return status;

	/* Setting imu struct values to converted measurements */
	imu.accel_data[LSM6DSO_X_AXIS] = accel_data_convert(accel_x_raw.data);
	imu.accel_data[LSM6DSO_Y_AXIS] = accel_data_convert(accel_y_raw.data);
	imu.accel_data[LSM6DSO_Z_AXIS] = accel_data_convert(accel_z_raw.data);

	return 0;
}

int lsm6dso_read_gyro()
{
	union {
		uint8_t buf[2];
		int16_t data;
	} gyro_x_raw, gyro_y_raw, gyro_z_raw;
	int status;

	/* Aquire raw data from registers */
	status = lsm6dso_read_mult_reg(gyro_x_raw.buf,
				       LSM6DSO_REG_GYRO_X_AXIS_L, 2);
	if (status != 0)
		return status;

	status = lsm6dso_read_mult_reg(gyro_y_raw.buf,
				       LSM6DSO_REG_GYRO_Y_AXIS_L, 2);
	if (status != 0)
		return status;

	status = lsm6dso_read_mult_reg(gyro_z_raw.buf,
				       LSM6DSO_REG_GYRO_Z_AXIS_L, 2);
	if (status != 0)
		return status;

	/* Setting imu struct values to converted measurements */
	imu.gyro_data[LSM6DSO_X_AXIS] = gyro_data_convert(gyro_x_raw.data);
	imu.gyro_data[LSM6DSO_Y_AXIS] = gyro_data_convert(gyro_y_raw.data);
	imu.gyro_data[LSM6DSO_Z_AXIS] = gyro_data_convert(gyro_z_raw.data);

	return 0;
}
