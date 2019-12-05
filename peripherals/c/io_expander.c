#include "io_expander.h"

//*****************************************************************************
// Used to determine if the io_expander is busy writing the last transaction to 
// non-volatile storage
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
// Returns
// I2C_OK is returned one the io_expander is ready to write the next byte
//*****************************************************************************
static 
i2c_status_t io_expander_wait_for_write( int32_t  i2c_base)
{
  
  i2c_status_t status;
  
  if( !i2cVerifyBaseAddr(i2c_base) )
  {
    return  I2C_INVALID_BASE;
  }

  // Set the I2C address to be the io_expander and in Write Mode
  status = i2cSetSlaveAddr(i2c_base, MCP23017_DEV_ID, I2C_WRITE);

  // Poll while the device is busy.  The  MCP24LC32AT will not ACK
  // writing an address while the write has not finished.
  do 
  {
    // The data we send does not matter.  This has been set to 0x00, but could
    // be set to anything
    status = i2cSendByte( i2c_base, 0x00, I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP);
    
    // Wait for the address to finish transmitting
    while ( I2CMasterBusy(i2c_base)) {};
    
    // If the address was not ACKed, try again.
  } while (I2CMasterAdrAck(i2c_base) == false);

  return  status;
}
  
  
//*****************************************************************************
// Writes a single byte of data out to the  MCP24LC32AT io_expander.  
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
//    address:    16-bit address of the byte being written.  Only the lower
//                12 bits is used by the io_expander
//
//    data:       Data written to the io_expander.
//
// Returns
// I2C_OK if the byte was written to the io_expander.
//*****************************************************************************
void io_expander_write_reg(uint8_t reg, uint8_t data)
{
  // Before doing anything, make sure the I2C device is idle
  while ( I2CMasterBusy(IO_EXPANDER_I2C_BASE)) {};

  //==============================================================
  // Set the I2C address to be the io_expander
  //==============================================================
  i2cSetSlaveAddr(IO_EXPANDER_I2C_BASE, MCP23017_DEV_ID, I2C_WRITE);
  // If the io_expander is still writing the last byte written, wait
  io_expander_wait_for_write(IO_EXPANDER_I2C_BASE);
  
  //==============================================================
  // Send the address
  //==============================================================
	i2cSendByte(IO_EXPANDER_I2C_BASE, reg, I2C_MCS_START | I2C_MCS_RUN);
  //==============================================================
  // Send the Byte of data to write
  //==============================================================
	i2cSendByte(IO_EXPANDER_I2C_BASE, data, I2C_MCS_RUN | I2C_MCS_STOP);
}

//*****************************************************************************
// Reads a single byte of data from the  MCP24LC32AT io_expander.  
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
//    address:    16-bit address of the byte being read.  Only the lower
//                12 bits is used by the io_expander
//
//    data:       data read from the io_expander is returned to a uint8_t pointer.
//
// Returns
// I2C_OK if the byte was read from the io_expander.
//*****************************************************************************
uint8_t io_expander_read_reg(uint8_t address)
{
  i2c_status_t status;
	uint8_t data;
  
  // Before doing anything, make sure the I2C device is idle
  while ( I2CMasterBusy(IO_EXPANDER_I2C_BASE)) {};
	
  // If the io_expander is still writing the last byte written, wait
  io_expander_wait_for_write(IO_EXPANDER_I2C_BASE);

  //==============================================================
  // Set the I2C slave address to be the io_expander and in Write Mode
  //==============================================================
	status = i2cSetSlaveAddr(IO_EXPANDER_I2C_BASE, MCP23017_DEV_ID, I2C_WRITE);
	if (status != I2C_OK) {
		return status;
	}
  //==============================================================
  // Send the address
  //==============================================================
	status = i2cSendByte(IO_EXPANDER_I2C_BASE, address, I2C_MCS_START | I2C_MCS_RUN);
	if (status != I2C_OK) {
		return status;
	}
  //==============================================================
  // Set the I2C slave address to be the io_expander and in Read Mode
	// ADD CODE
  //==============================================================
	status = i2cSetSlaveAddr(IO_EXPANDER_I2C_BASE, MCP23017_DEV_ID, I2C_READ);
	if (status != I2C_OK) {
		return status;
	}
  //==============================================================
  // Read the data returned by the io_expander
  //==============================================================
  status = i2cGetByte(IO_EXPANDER_I2C_BASE, &data, I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP);
  return data;
}

//*****************************************************************************
// Initialize the I2C peripheral
//*****************************************************************************
bool io_expander_init(void)
{
	uint8_t reg;
  if(gpio_enable_port(IO_EXPANDER_GPIO_BASE) == false)
  {
    return false;
  }
  
  // Configure SCL 
  if(gpio_config_digital_enable(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SCL_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_alternate_function(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SCL_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_port_control(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SCL_PCTL_M, IO_EXPANDER_I2C_SCL_PIN_PCTL)== false)
  {
    return false;
  }
    

  
  // Configure SDA 
  if(gpio_config_digital_enable(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_open_drain(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_alternate_function(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_port_control(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PCTL_M, IO_EXPANDER_I2C_SDA_PIN_PCTL)== false)
  {
    return false;
  }
    
  //  Initialize the I2C peripheral
  if( initializeI2CMaster(IO_EXPANDER_I2C_BASE)!= I2C_OK)
  {
    return false;
  }
  
	// CONFIGURE BUTTONS
	io_expander_write_reg(MCP23017_IODIRB_R, 0xFF);
	io_expander_write_reg(MCP23017_GPPUB_R, 0xFF);
	
	reg = io_expander_read_reg(MCP23017_INTCAPB_R);
	reg = io_expander_read_reg(MCP23017_GPIOB_R);
	
	io_expander_write_reg(MCP23017_GPINTENB_R, 0x0F);
	io_expander_write_reg(MCP23017_DEFVALB_R, 0x0F);
	io_expander_write_reg(MCP23017_INTCONB_R, 0x00);
	
	io_expander_write_reg(MCP23017_IOCONB_R, 0x00);
	
	reg = io_expander_read_reg(MCP23017_INTCAPB_R);
	reg = io_expander_read_reg(MCP23017_GPIOB_R);
	
	// CONFIGURE LEDS
	io_expander_write_reg(MCP23017_IODIRA_R, 0x00);
	
	
  return true;
}
