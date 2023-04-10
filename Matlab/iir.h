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

#ifndef __IIR_H__
#define __IIR_H__

/*--- Public dependencies -------------------------------------------------------------*/
#include <stdint.h>

#include "iir_port.h"

#ifdef __cplusplus
extern "C" {
#endif

/*--- Public macros -------------------------------------------------------------------*/

/*--- Public type definitions ---------------------------------------------------------*/
typedef struct iir* iir_t;
typedef float		FLOAT;

/*--- Public variable declarations ----------------------------------------------------*/

/*--- Public function declarations ----------------------------------------------------*/

/**
 * @brief
 *
 * @param order
 * @param a
 * @param b
 * @param init_output
 * @return iir_t
 */
iir_t iir_new(uint8_t order, FLOAT* a, FLOAT* b, FLOAT init_output);

/**
 * @brief
 *
 * @param p_iir
 */
void iir_del(iir_t* p_iir);

/**
 * @brief
 *
 * @param iir
 * @param init_output
 */
void iir_reset(iir_t iir, FLOAT init_output);

/**
 * @brief
 *
 * @param iir
 * @param input
 * @return FLOAT
 */
FLOAT iir_step(iir_t iir, FLOAT input);

#ifdef __cplusplus
}
#endif

#endif	// __IIR_H__
