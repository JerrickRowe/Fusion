/*
 * File: filter_butter.c
 *
 * MATLAB Coder version            : 5.4
 * C/C++ source code generated on  : 05-Mar-2023 22:50:00
 */

/* Include Files */
#include "filter_butter.h"
#include "filter_butter_types.h"
#include "rt_nonfinite.h"
#include "rt_nonfinite.h"

/* Function Definitions */
/*
 * Arguments    : const struct0_T *f
 *                double in
 * Return Type  : double
 */
double filter_butter(const struct0_T *f, double in)
{
  double b[7];
  int k;
  for (k = 0; k < 7; k++) {
    b[k] = f->b[k];
  }
  if ((!rtIsInf(f->a[0])) && (!rtIsNaN(f->a[0])) && (!(f->a[0] == 0.0)) &&
      (f->a[0] != 1.0)) {
    for (k = 0; k < 7; k++) {
      b[k] /= f->a[0];
    }
  }
  return in * b[0];
}

/*
 * File trailer for filter_butter.c
 *
 * [EOF]
 */
