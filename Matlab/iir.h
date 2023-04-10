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
 * @brief Create an IIR-Filter object
 *
 * @param len Length of coefficients of the filter, which is order+1
 * @param a Coefficient a of the filter H(z) = B(z) / A(z)
 * @param b Coefficient b of the filter H(z) = B(z) / A(z)
 * @return iir_t Filter object
 */
iir_t iir_new(uint8_t len, FLOAT* a, FLOAT* b);

/**
 * @brief Delete the filter object
 *
 * @param p_iir Pointer to the filter object
 */
void iir_del(iir_t* p_iir);

/**
 * @brief Reset the filter to initial state
 *
 * @param iir Filter object
 * @param init_vector Pointer to the init vector
 * @param len Length of init vector, must be the same as the length of the filter itself
 */
void iir_reset(iir_t iir, FLOAT* init_z, uint8_t len);

/**
 * @brief Filter 1 sample of data
 *
 * @param iir Filter object
 * @param input Sample of data
 * @return FLOAT Filtered data
 */
FLOAT iir_step(iir_t iir, FLOAT input);

#ifdef __cplusplus
}
#endif

#endif	// __IIR_H__
