/*
 * IRdecodePanavoxAC.cpp
 *
 *  Created on: 11 dic. 2016
 *      Author: Gabriel Roldán
 */

#include "IRPanavoxAC.h"
#include "IRdecodePanavoxAC.h"

IRdecodePanavoxAC::~IRdecodePanavoxAC(void) {

}

int IRdecodePanavoxAC::readNextByte(uint8_t offset) {
	uint8_t data = 0;
	uint8_t offset_limit = offset + 16;

	while (offset < offset_limit) {

		if (!MATCH(this->rawbuf[offset], PANAVOX_AC_BIT_MARK)) {
			return READ_ERROR;
		}

		offset++;

		if (MATCH(this->rawbuf[offset], PANAVOX_AC_ONE_SPACE)) {
			data = (data << 1) | 1;
		} else if (MATCH(this->rawbuf[offset], PANAVOX_AC_ZERO_SPACE)) {
			data <<= 1;
		} else {
			return READ_ERROR;
		}

		offset++;
	}

	return data;
}

bool IRdecodePanavoxAC::decode(void) {

	uint32_t data[6];
	uint8_t offset;

	if (MATCH(this->rawbuf[0], PANAVOX_AC_RPT_SPACE))
		this->repeat = true;
	else
		this->repeat = false;

	if (this->rawlen != PANAVOX_RAW_COUNT)
		return RAW_COUNT_ERROR;

	// Check header
	if (!MATCH(this->rawbuf[1], PANAVOX_AC_HDR_MARK))
		return HEADER_MARK_ERROR(PANAVOX_AC_HDR_MARK);

	if (!MATCH(this->rawbuf[2], PANAVOX_AC_HDR_SPACE))
		return HEADER_SPACE_ERROR(PANAVOX_AC_HDR_SPACE);

	offset = 3; // Skip header and initial gap

	// Read data
	for (int i = 0; i < 6; i++) {
		int result = this->readNextByte(offset + (i * 16));
		if (result == READ_ERROR)
			return false;

		data[i] = (unsigned char) result;
	}

	// Check data integrity
	for (int i = 0; i < 6; i += 2) {
		if ((~data[i] & 0xFF) != data[i + 1])
			return false;
	}

	this->value = (data[4] & 0xFF) << 16 | (data[2] & 0xFF) << 8
			| (data[0] & 0xFF);

	this->decode_type = PANAVOX_AC;
	this->bits = 24;

	return true;
}

void IRdecodePanavoxAC::DumpCmdModeResults(void) {
	// Dump configuration data
	panavox_ac_cmd_mode_t* cmd = (panavox_ac_cmd_mode_t*) &this->value;

	Serial.print(F("  param1.ext = 0x"));
	Serial.println(cmd->param1.ext, HEX);
	Serial.print(F("  param1.fan_speed = 0x"));
	Serial.println(cmd->param1.fan_speed, HEX);
	Serial.print(F("  param2.ext = 0x"));
	Serial.println(cmd->param2.ext, HEX);
	Serial.print(F("  param2.mode = 0x"));
	Serial.println(cmd->param2.mode, HEX);
	Serial.print(F("  param2.temp = 0x"));
	Serial.print(cmd->param2.temp, HEX);
	Serial.print(F(" ("));

	uint8_t temp = cmd->param2.temp;
	temp = decodeTemp(temp);
	Serial.print(temp);
	Serial.println(F(" C)"));
}

void IRdecodePanavoxAC::DumpResults(void) {

	if (this->repeat && !this->dump_repeat)
		return;

	Serial.print(F("Value: "));
	Serial.println(this->value, HEX);

	if (this->dump_repeat) {
		Serial.print(F("Repeat: "));
		if (this->repeat)
			Serial.println(F("YES"));
		else
			Serial.println(F("NO"));
	}

	// Dump configuration data
	panavox_ac_cmd_t* cmd = (panavox_ac_cmd_t*) &this->value;

	Serial.print(F("Command type: "));
	if (cmd->type == PANAVOX_AC_CMD_MODE) {
		Serial.println(F("MODE SET"));
		this->DumpCmdModeResults();
	} else if (cmd->type == PANAVOX_AC_CMD_EXTRA) {
		Serial.println(F("MODE EXTRA"));
	} else
		Serial.println(F("UNKWNON"));

	Serial.println();
}

bool IRdecodePanavoxAC::decodeState(panavox_ac_mode_state_t* state) {
	panavox_ac_cmd_mode_t* cmd = (panavox_ac_cmd_mode_t*) &this->value;
	uint8_t FAN_SPEEDS[] = { PANAVOX_AC_FAN_SPEED_0, PANAVOX_AC_FAN_SPEED_1,
	PANAVOX_AC_FAN_SPEED_2, PANAVOX_AC_FAN_SPEED_3 };
	uint8_t fan_speed;

	if (cmd->type != PANAVOX_AC_CMD_MODE)
		return false;

	for (int speed = 0; speed < 4; speed++) {
		if (FAN_SPEEDS[speed] == cmd->param1.fan_speed)
			fan_speed = speed;
	}

	// Clean values
	state->temp = -1;
	state->fan_speed = -1;

	if (cmd->param2.mode == PANAVOX_AC_MODE_AUTO) {
		state->mode = AC_MODE_AUTO;
		state->temp = decodeTemp(cmd->param2.temp);
	} else if (cmd->param2.mode == PANAVOX_AC_MODE_COOL) {
		state->mode = AC_MODE_COOL;
		state->temp = decodeTemp(cmd->param2.temp);
		state->fan_speed = fan_speed;
	} else if (cmd->param2.mode == PANAVOX_AC_MODE_HEAT) {
		state->mode = AC_MODE_HEAT;
		state->temp = decodeTemp(cmd->param2.temp);
		state->fan_speed = fan_speed;
	} else if (cmd->param2.mode == PANAVOX_AC_MODE_DRY_FAN) {
		if (cmd->param2.temp == 0x0E) {
			state->mode = AC_MODE_FAN;
			state->fan_speed = fan_speed;
		} else {
			state->mode = AC_MODE_DRY;
			state->temp = decodeTemp(cmd->param2.temp);
		}
	} else {
		return false;
	}

	return true;
}

uint8_t IRdecodePanavoxAC::decodeTemp(uint8_t value) {
	uint8_t result = value;

	if (result == 8)
		result = 14;
	if (result == 9)
		result = 15;

	result = result ^ (result >> 2);
	result = result ^ (result >> 1);

	return result + 17;
}
