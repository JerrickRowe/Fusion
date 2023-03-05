/*
 * File: _coder_filter_butter_api.h
 *
 * MATLAB Coder version            : 5.4
 * C/C++ source code generated on  : 05-Mar-2023 22:50:00
 */

#ifndef _CODER_FILTER_BUTTER_API_H
#define _CODER_FILTER_BUTTER_API_H

/* Include Files */
#include "emlrt.h"
#include "tmwtypes.h"
#include <string.h>

/* Type Definitions */
#ifndef typedef_struct0_T
#define typedef_struct0_T
typedef struct {
  real_T b[7];
  real_T a[7];
} struct0_T;
#endif /* typedef_struct0_T */

/* Variable Declarations */
extern emlrtCTX emlrtRootTLSGlobal;
extern emlrtContext emlrtContextGlobal;

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
real_T filter_butter(struct0_T *f, real_T in);

void filter_butter_api(const mxArray *const prhs[2], const mxArray **plhs);

void filter_butter_atexit(void);

void filter_butter_initialize(void);

void filter_butter_terminate(void);

void filter_butter_xil_shutdown(void);

void filter_butter_xil_terminate(void);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for _coder_filter_butter_api.h
 *
 * [EOF]
 */
