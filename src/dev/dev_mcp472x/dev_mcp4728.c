// Copyright 2021 Ocean (iiot2k@gmail.com) 
// All rights reserved.

#include "dev_mcp4728.h"

//#define MCP4728_ADDR 0x60
#define MULTIWRITE		0x40
#define SINGLEWRITE		0x58
#define FASTWRITE		0xC0

static uint8_t dac_pd[] = { MCP4728_PD_OFF, MCP4728_PD_OFF, MCP4728_PD_OFF, MCP4728_PD_OFF };
static uint8_t dac_vref[] = { MCP4728_VREF_VDD, MCP4728_VREF_VDD, MCP4728_VREF_VDD, MCP4728_VREF_VDD };
static uint8_t dac_gain[] = { MCP4728_GAIN2, MCP4728_GAIN2, MCP4728_GAIN2, MCP4728_GAIN2 };

void dev_mcp4728_pd(uint8_t addr, uint8_t ch, uint8_t pd)
{
	if ((ch >= MCP4728_CH_NUM) || (pd >= MCP4728_PD_NUM))
		return;

	dac_pd[ch] = pd;
}

void dev_mcp4728_vref(uint8_t addr, uint8_t ch, uint8_t vref)
{
	if ((ch >= MCP4728_CH_NUM) || (vref >= MCP4728_VREF_NUM))
		return;

	dac_vref[ch] = vref;
}

void dev_mcp4728_gain(uint8_t addr, uint8_t ch, uint8_t gain)
{
	if ((ch >= MCP4728_CH_NUM) || (gain >= MCP4728_GAIN_NUM))
		return;
		
	dac_gain[ch] = gain;
}

static bool mcp4728_write(i2c_inst_t* i2c, uint8_t addr, uint8_t cmd, uint8_t ch, uint16_t value)
{
	if (ch >= MCP4728_CH_NUM)
		return false;

	if (value > 4095)
		value = 4095;
	
	uint8_t buffer[3];

	buffer[0] = cmd | (ch << 1);
	buffer[1] = (value >> 8) | (dac_vref[ch] << 7) | (dac_pd[ch] << 5) | (dac_gain[ch] << 4);
	buffer[2] = value & 0xFF;

	// set channel dac
    return (sys_i2c_wbuf(i2c, addr, buffer, sizeof(buffer)) == sizeof(buffer));
}

bool dev_mcp4728_write_fast(i2c_inst_t* i2c, uint8_t addr, uint16_t value0, uint16_t value1, uint16_t value2, uint16_t value3)
{
	uint8_t buffer[8];

	buffer[0] = (value0 >> 8) | (0x00 << 6) | (dac_pd[0] << 5);
	buffer[1] = value0 & 0xFF;
	buffer[2] = (value1 >> 8) | (dac_pd[1] << 5);
	buffer[3] = value1 & 0xFF;
	buffer[4] = (value2 >> 8) | (dac_pd[2] << 5);
	buffer[5] = value2 & 0xFF;
	buffer[6] = (value3 >> 8) | (dac_pd[3] << 5);
	buffer[7] = value3 & 0xFF;

	return (sys_i2c_wbuf(i2c, addr, buffer, sizeof(buffer)) == sizeof(buffer));
}

bool dev_mcp4728_set(i2c_inst_t* i2c, uint8_t addr, uint8_t ch, uint16_t value)
{
	return mcp4728_write(i2c, addr, MULTIWRITE, ch, value);
}

bool dev_mcp4728_save(i2c_inst_t* i2c, uint8_t addr, uint8_t ch, uint16_t value)
{
	return mcp4728_write(i2c, addr, SINGLEWRITE, ch, value);
}
