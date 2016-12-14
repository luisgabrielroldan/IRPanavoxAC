/*
 * IRPanavoxAC.h
 *
 *  Created on: 10 dic. 2016
 *      Author: Gabriel Roldán
 */

#ifndef _IR_PANAVOX_AC_H_
#define _IR_PANAVOX_AC_H_

#include <IRLib.h>
#include <IRLibMatch.h>

#define PANAVOX_AC				LAST_PROTOCOL + 10

#define PANAVOX_AC_FREQ         38

#define PANAVOX_RAW_COUNT		100

#define PANAVOX_AC_HDR_MARK		4300
#define PANAVOX_AC_HDR_SPACE	4500
#define PANAVOX_AC_BIT_MARK		500
#define PANAVOX_AC_ONE_SPACE	1600
#define PANAVOX_AC_ZERO_SPACE	600
#define PANAVOX_AC_RPT_SPACE	5400
#define PANAVOX_AC_END_SPACE	0

#define PANAVOX_AC_FAN_SPEED_0  5
#define PANAVOX_AC_FAN_SPEED_1  4
#define PANAVOX_AC_FAN_SPEED_2  2
#define PANAVOX_AC_FAN_SPEED_3  1

#define PANAVOX_AC_SIGN         0x0B
#define PANAVOX_AC_CMD_MODE		2
#define PANAVOX_AC_CMD_EXTRA	5

typedef enum ac_mode_t {
	AC_MODE_OFF = 0,
	AC_MODE_AUTO = 1,
	AC_MODE_COOL = 2,
	AC_MODE_HEAT = 3,
	AC_MODE_DRY = 4,
	AC_MODE_FAN = 5
} ac_mode_t;

typedef struct panavox_ac_cmd_t {
	uint8_t type :4;
	uint8_t sign :4;
	uint8_t param1;
	uint8_t param2;
} panavox_ac_cmd_t;

typedef struct panavox_ac_cmd_mode_param1_t {
	uint8_t ext :5; // Fill with ones
	uint8_t fan_speed :3;
} panavox_ac_cmd_mode_param1_t;

typedef struct panavox_ac_cmd_mode_param2_t {
	uint8_t ext :2;
	uint8_t mode :2;
	uint8_t temp :4;
} panavox_ac_cmd_mode_param2_t;

typedef struct panavox_ac_cmd_mode_t {
	uint8_t type :4;
	uint8_t sign :4;
	panavox_ac_cmd_mode_param1_t param1;
	panavox_ac_cmd_mode_param2_t param2;
} panavox_ac_cmd_mode_t;

typedef enum {
	PANAVOX_AC_MODE_COOL = 0,
	PANAVOX_AC_MODE_DRY_FAN = 1,
	PANAVOX_AC_MODE_AUTO = 2,
	PANAVOX_AC_MODE_HEAT = 3
} panavox_ac_mode_t;

typedef struct panavox_ac_mode_state_t {
	ac_mode_t mode;
	int8_t fan_speed;
	int8_t temp;
} panavox_ac_mode_state_t;

#include "IRdecodePanavoxAC.h"
#include "IRsendPanavoxAC.h"

#endif /* _IR_PANAVOX_AC_H_ */
