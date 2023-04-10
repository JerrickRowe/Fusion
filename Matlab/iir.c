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
	float	*x;
	float	*y;
	float	*a;
	float	*b;
};

/*--- Private variable definitions -----------------------------------------------*/

/*--- Private function declarations ----------------------------------------------*/

/*--- Private function definitions -----------------------------------------------*/

/*--- Public function definitions ------------------------------------------------*/

iir_t iir_new(uint8_t len, FLOAT *a, FLOAT *b, FLOAT init_output) {
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
	ret->x = (FLOAT *)MALLOC(sizeof(FLOAT) * len);
	ASSERT(ret->x);
	ret->y = (FLOAT *)MALLOC(sizeof(FLOAT) * len);
	ASSERT(ret->y);
	for(int i = 0; i < len; i++) {
		ret->a[i] = a[i];
		ret->b[i] = b[i];
	}
	ret->len		= len;
	ret->magic_word = MAGIC_WORD;
	iir_reset(ret, init_output);
	return ret;
}

void iir_del(iir_t *p_iir) {
	ASSERT(p_iir);
	ASSERT(*p_iir);
	ASSERT((*p_iir)->magic_word == MAGIC_WORD);
	ASSERT((*p_iir)->a);
	ASSERT((*p_iir)->b);
	ASSERT((*p_iir)->y);
	FREE((*p_iir)->a);
	FREE((*p_iir)->b);
	FREE((*p_iir)->y);
	FREE(*p_iir);
	*p_iir = NULL;
}

void iir_reset(iir_t iir, FLOAT init_output) {
	ASSERT(iir);
	ASSERT(iir->magic_word == MAGIC_WORD);
	for(int i = 0; i < iir->len; i++) {
		iir->y[i] = init_output;
	}
}

FLOAT iir_step(iir_t iir, FLOAT input) {
	ASSERT(iir->magic_word == MAGIC_WORD);
	FLOAT b[7];
	for(int k = 0; k < iir->len; k++) {
		b[k] = iir->b[k];
	}
	// if((!rtIsInf(iir->a[0])) && (!rtIsNaN(iir->a[0])) && (!(iir->a[0] == 0.0))
	//    && (iir->a[0] != 1.0)) {
	// if(!(iir->a[0] == 0.0) && (iir->a[0] != 1.0)) {
		for(int k = 0; k < 7; k++) {
			b[k] /= iir->a[0];
		}
	// }
	return (FLOAT)input * b[0];
}

#ifdef __cplusplus
}
#endif
