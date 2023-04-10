/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) [2023] [Jerrick.Rowe]                                            */
/*                                                                                */
/* Permission is hereby granted, free of charge, to any person obtaining a copy   */
/* of this software and associated documentation files (the "Software"), to deal  */
/* in the Software without restriction, including without limitation the rights   */
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      */
/* copies of the Software, and to permit persons to whom the Software is          */
/* furnished to do so, subject to the following conditions:                       */
/*                                                                                */
/* The above copyright notice and this permission notice shall be included in all */
/* copies or substantial portions of the Software.                                */
/*                                                                                */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  */
/* SOFTWARE.                                                                      */
/**********************************************************************************/

/*--- Private dependencies -------------------------------------------------------*/
#include "iir.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*--- Public variable definitions ------------------------------------------------*/

/*--- Private macros -------------------------------------------------------------*/
#ifndef IIR_PORT_MALLOC
#include <stdlib.h>
#define IIR_PORT_MALLOC(_want) malloc(_want)
#endif

#ifndef IIR_PORT_MALLOC
#include <stdlib.h>
#define IIR_PORT_MALLOC(_p) free(_p)
#endif

#ifndef IIR_PORT_MALLOC
#include <assert.h>
#define IIR_PORT_MALLOC(_e) assert(_e)
#endif

#define MALLOC(_want) IIR_PORT_MALLOC((_want))
#define FREE(_p)	  IIR_PORT_FREE((_p))
#define ASSERT(_e)	  IIR_PORT_ASSERT((_e))

#define MAGIC_WORD (0x13657071)

/*--- Private type definitions ---------------------------------------------------*/
struct iir {
	uint32_t magic_word;
	uint8_t	 len;
	float	*z;
	float	*a;
	float	*b;
};

/*--- Private variable definitions -----------------------------------------------*/

/*--- Private function declarations ----------------------------------------------*/

/*--- Private function definitions -----------------------------------------------*/

/*--- Public function definitions ------------------------------------------------*/

iir_t iir_new(uint8_t len, FLOAT *a, FLOAT *b) {
	ASSERT(len);
	ASSERT(a);
	ASSERT(b);
	iir_t ret = NULL;
	ret		  = MALLOC(sizeof(struct iir));
	ASSERT(ret);
	ret->a = (FLOAT *)MALLOC(sizeof(FLOAT) * len);
	ASSERT(ret->a);
	ret->b = (FLOAT *)MALLOC(sizeof(FLOAT) * len);
	ASSERT(ret->b);
	ret->z = (FLOAT *)MALLOC(sizeof(FLOAT) * len);
	ASSERT(ret->z);
	for(int i = 0; i < len; i++) {
		ret->a[i] = a[i];
		ret->b[i] = b[i];
		ret->z[i] = 0;
	}
	ret->len		= len;
	ret->magic_word = MAGIC_WORD;
	return ret;
}

void iir_del(iir_t *p_iir) {
	ASSERT(p_iir);
	ASSERT(*p_iir);
	ASSERT((*p_iir)->magic_word == MAGIC_WORD);
	ASSERT((*p_iir)->a);
	ASSERT((*p_iir)->b);
	ASSERT((*p_iir)->z);
	FREE((*p_iir)->a);
	FREE((*p_iir)->b);
	FREE((*p_iir)->z);
	FREE(*p_iir);
	*p_iir = NULL;
}

void iir_reset(iir_t iir, FLOAT *init_z, uint8_t len) {
	ASSERT(iir);
	ASSERT(iir->magic_word == MAGIC_WORD);
	ASSERT(iir->len == len);
	for(int i = 0; i < iir->len; i++) {
		iir->z[i] = init_z[i];
	}
}

FLOAT iir_step(iir_t iir, FLOAT input) {
	ASSERT(iir->magic_word == MAGIC_WORD);
	uint8_t order = iir->len - 1;
	FLOAT	Yi	  = iir->b[0] * input + iir->z[0];	// Filtered value
	for(int j = 1; j < order; j++) {				// Update conditions
		iir->z[j - 1] = iir->b[j] * input + iir->z[j] - iir->a[j] * Yi;
	}
	iir->z[order - 1] = iir->b[order] * input - iir->a[order] * Yi;
	return Yi;	// Write to output
}

#ifdef __cplusplus
}
#endif
