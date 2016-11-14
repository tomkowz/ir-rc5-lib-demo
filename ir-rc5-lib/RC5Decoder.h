#ifndef _RC5DECODER_H_
#define _RC5DECODER_H_

#include "IRCode.h"

class RC5Decoder {
public:
	static IRCode* decode(unsigned long int* buffer, unsigned int bufferLength);
};

#endif
