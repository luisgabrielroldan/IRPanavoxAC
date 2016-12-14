/*
 * IRdecodePanavoxAC.h
 *
 *  Created on: 11 dic. 2016
 *      Author: Gabriel Roldán
 */

#ifndef _IR_DECODE_PANAVOX_AC_H_
#define _IR_DECODE_PANAVOX_AC_H_

#define READ_ERROR		-1

class IRdecodePanavoxAC: public virtual IRdecodeBase {
private:
	int readNextByte(uint8_t offset);
	uint8_t decodeTemp(uint8_t value);

	void DumpCmdModeResults(void);
public:
	bool repeat; // Is repeated data
	bool dump_repeat; // Show repeated data in dump
	bool decodeState(panavox_ac_mode_state_t* state);
	virtual bool decode(void);
	virtual void DumpResults(void);
	virtual ~IRdecodePanavoxAC(void);
};

#endif /* _IR_DECODE_PANAVOX_AC_H_ */
