/*
 * IRsendPanavoxAC.cpp
 *
 *  Created on: 11 dic. 2016
 *      Author: Gabriel Roldán
 */

#include "IRPanavoxAC.h"
#include "IRsendPanavoxAC.h"

void IRsendPanavoxAC::sendPacket(uint32_t value) {
	uint8_t* data = (uint8_t*) &value;

	mark(PANAVOX_AC_HDR_MARK);
	space(PANAVOX_AC_HDR_SPACE);

	for (int i = 0; i < 3; i++) {
		sendByte(data[i]);
		sendByte(~data[i]);
	}

	mark(PANAVOX_AC_BIT_MARK);
}

void IRsendPanavoxAC::send(uint32_t* value) {
	enableIROut(PANAVOX_AC_FREQ);

	sendPacket(*value);
	space(PANAVOX_AC_RPT_SPACE);
	sendPacket(*value);
	space(PANAVOX_AC_END_SPACE);
}

void IRsendPanavoxAC::sendByte(uint8_t data) {
	for (int i = 7; i >= 0; i--) {
		mark(PANAVOX_AC_BIT_MARK);
		if (CHECK_BIT(data, i)) {
			space(PANAVOX_AC_ONE_SPACE);
		} else {
			space(PANAVOX_AC_ZERO_SPACE);
		}
	}
}

uint8_t IRsendPanavoxAC::encodeTemp(uint8_t temp) {
	uint8_t value = temp - 17;
	if (value == 10 || value == 11)
		value += 4;
	return value ^ (value >> 1);
}

bool IRsendPanavoxAC::isValidState(panavox_ac_mode_state_t* state) {
	if (state->fan_speed > 3) {
		return false;
	}

	if (state->temp < 17 || state->temp > 30) {
		return false;
	}

	return true;
}

void IRsendPanavoxAC::sendMoveFlap(void) {
	panavox_ac_cmd_t cmd;

	cmd.sign = PANAVOX_AC_SIGN;
	cmd.type = PANAVOX_AC_CMD_MODE;
	cmd.param1 = 0x0F;
	cmd.param2 = 0xE0;

	send((uint32_t*) &cmd);
}

void IRsendPanavoxAC::sendSwapLight(void) {
	panavox_ac_cmd_t cmd;

	cmd.sign = PANAVOX_AC_SIGN;
	cmd.type = PANAVOX_AC_CMD_EXTRA;
	cmd.param1 = 0xF5;
	cmd.param2 = 0xA5;

	send((uint32_t*) &cmd);
}

void IRsendPanavoxAC::sendSuperMode(void) {
	panavox_ac_cmd_t cmd;

	cmd.sign = PANAVOX_AC_SIGN;
	cmd.type = PANAVOX_AC_CMD_EXTRA;
	cmd.param1 = 0xF5;
	cmd.param2 = 0xA2;

	send((uint32_t*) &cmd);
}

void IRsendPanavoxAC::sendAutoClean(void) {
	panavox_ac_cmd_t cmd;

	cmd.sign = PANAVOX_AC_SIGN;
	cmd.type = PANAVOX_AC_CMD_EXTRA;
	cmd.param1 = 0xF5;
	cmd.param2 = 0xAA;

	send((uint32_t*) &cmd);
}

void IRsendPanavoxAC::sendFlapOscillation(void) {
	panavox_ac_cmd_t cmd;

	cmd.sign = PANAVOX_AC_SIGN;
	cmd.type = PANAVOX_AC_CMD_MODE;
	cmd.param1 = 0x6B;
	cmd.param2 = 0xE0;

	send((uint32_t*) &cmd);
}

void IRsendPanavoxAC::sendShutdown(void) {
	panavox_ac_cmd_t cmd;

	cmd.sign = PANAVOX_AC_SIGN;
	cmd.type = PANAVOX_AC_CMD_MODE;
	cmd.param1 = 0x7B;
	cmd.param2 = 0xE0;

	send((uint32_t*) &cmd);
}

bool IRsendPanavoxAC::sendModeCmd(panavox_ac_mode_state_t* state) {
	uint8_t FAN_SPEEDS[] = { PANAVOX_AC_FAN_SPEED_0, PANAVOX_AC_FAN_SPEED_1,
	PANAVOX_AC_FAN_SPEED_2, PANAVOX_AC_FAN_SPEED_3 };

	panavox_ac_cmd_t cmd;
	panavox_ac_cmd_mode_t* cmd_mode = (panavox_ac_cmd_mode_t*) &cmd;

	if (!isValidState(state))
		return false;

	cmd.sign = PANAVOX_AC_SIGN;
	cmd.type = PANAVOX_AC_CMD_MODE;

	cmd_mode->param1.ext = 0x1F;
	cmd_mode->param2.ext = 0;

	cmd_mode->param1.fan_speed = FAN_SPEEDS[state->fan_speed];
	cmd_mode->param2.temp = encodeTemp(state->temp);

	if (state->mode == AC_MODE_OFF) {
		cmd.param1 = 0x7B;
		cmd.param2 = 0xE0;
	} else if (state->mode == AC_MODE_AUTO) {
		cmd_mode->param2.mode = PANAVOX_AC_MODE_AUTO;
		cmd_mode->param1.fan_speed = 0;
	} else if (state->mode == AC_MODE_DRY) {
		cmd_mode->param2.mode = PANAVOX_AC_MODE_DRY_FAN;
		cmd_mode->param1.fan_speed = 0;
	} else if (state->mode == AC_MODE_FAN) {
		cmd_mode->param2.mode = PANAVOX_AC_MODE_DRY_FAN;
		cmd_mode->param2.temp = 0x0E;
	} else if (state->mode == AC_MODE_COOL) {
		cmd_mode->param2.mode = PANAVOX_AC_MODE_COOL;
	} else if (state->mode == AC_MODE_HEAT) {
		cmd_mode->param2.mode = PANAVOX_AC_MODE_HEAT;
	} else
		return false;

	send((uint32_t*) &cmd);
	return true;
}
