/*
 * IRsendPanavoxAC.h
 *
 *  Created on: 11 dic. 2016
 *      Author: Gabriel Roldán
 */

#ifndef _IR_SEND_PANAVOX_AC_H_
#define _IR_SEND_PANAVOX_AC_H_

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

class IRsendPanavoxAC: public virtual IRsendBase {
public:
	void send(uint32_t* value);
	bool sendModeCmd(panavox_ac_mode_state_t* state);
	void sendMoveFlap(void);
	void sendSwapLight(void);
	void sendSuperMode(void);
	void sendAutoClean(void);
	void sendFlapOscillation(void);
	void sendShutdown(void);
private:
	void sendByte(uint8_t data);
	void sendPacket(uint32_t value);
	uint8_t encodeTemp(uint8_t temp);
	bool isValidState(panavox_ac_mode_state_t* state);
};

#endif /* _IR_SEND_PANAVOX_AC_H_ */
