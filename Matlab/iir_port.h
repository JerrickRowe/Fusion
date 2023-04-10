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

#ifndef __IIR_PORT_H__
#define __IIR_PORT_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined __ARMCC_VERSION

#include "sysheap.h"
#define IIR_PORT_MALLOC(_want) malloc(_want)
#define IIR_PORT_FREE(_p)	   free(_p)

#include <assert.h>
#define IIR_PORT_ASSERT(_e) assert(_e)

#else

#include <stdlib.h>
#define IIR_PORT_MALLOC(_want) malloc(_want)
#define IIR_PORT_FREE(_p)	   free(_p)

#include <assert.h>
#define IIR_PORT_ASSERT(_e) assert(_e)

#endif

#ifdef __cplusplus
}
#endif

#endif	// __IIR_PORT_H__
