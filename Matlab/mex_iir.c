#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "mex.h"
#include "matrix.h"

#include "iir.h"

#define LENGTH (7)

static FLOAT coef_a[LENGTH] = {
	1,
	-4.06164399921344,
	7.09950381875073,
	-6.78501602539753,
	3.72301942889162,
	-1.10867085534354,
	0.139660041747140,
};

static FLOAT coef_b[LENGTH] = {
	0.000107068897421402, 0.000642413384528415, 0.00160603346132104,  0.00214137794842805,
	0.00160603346132104,  0.000642413384528415, 0.000107068897421402,
};

void CoreSingleN(float	*X,
				 size_t	 MX,
				 size_t	 NX,
				 double *a,
				 double *b,
				 uint8_t order,
				 double *Z,
				 float	*Y) {
	// Method for general filter length.
	// INPUT:
	//   X:  SINGLE array. Operation happens of 1st dimension.
	//   MX: Number of elements in the 1st dimension
	//   NX: Number of columns, considers mutliple dimensions.
	//   a, b: DOUBLE vector of filter parameters. Both have (order+1) elements.
	//       The first element of a is 1.
	//   Z:  DOUBLE (!) array, initial conditions.
	//   order: Number of filter parameters.
	// OUTPUT:
	//   Z:  DOUBLE (!) array, final conditions.
	//   Y:  SINGLE array, allocated by the caller.
	//
	// All intermediate values are DOUBLEs to increase the accuracy.
	// This is 30% faster than the DOUBLE method.

	double Xi, Yi;
	mwSize i, j, R;

	i = 0;
	while(NX--) {
		R = i + MX;
		while(i < R) {
			Xi = X[i];
			Yi = b[0] * Xi + Z[0];
			for(j = 1; j < order; j++) {
				Z[j - 1] = b[j] * Xi + Z[j] - a[j] * Yi;
			}
			Z[order - 1] = b[order] * Xi - a[order] * Yi;

			Y[i++] = (float)Yi;
		}
		Z += order;
	}

	return;
}

void CoreDoubleN(double *X,
				 size_t	 MX,
				 size_t	 NX,
				 double *a,
				 double *b,
				 uint8_t order,
				 double *Z,
				 double *Y) {
	// Direct form II transposed method for general filter length.
	// Implemented as time domain difference equations.
	// INPUT:
	//   X:  Double array. Operation happens of 1st dimension.
	//   MX: Number of elements in the 1st dimension
	//   NX: Number of columns, considers mutliple dimensions.
	//   a, b: Double vector of filter parameters. Both have nParam elements.
	//       The first element of a is 1.
	//   Z:  DOUBLE array, initial conditions.
	//   nParam: Number of filter parameters, order of filter + 1.
	// OUTPUT:
	//   Z:  DOUBLE array, final conditions.
	//   Y:  Double array, allocated by the caller.

	double Xi, Yi;
	mwSize i, j, R;

	i = 0;
	while(NX--) {	 // Next slice
		R = i + MX;	 // End of the column
		while(i < R) {
			Xi = X[i];					  // Get signal
			Yi = b[0] * Xi + Z[0];		  // Filtered value
			for(j = 1; j < order; j++) {  // Update conditions
				Z[j - 1] = b[j] * Xi + Z[j] - a[j] * Yi;
			}
			Z[order - 1] = b[order] * Xi - a[order] * Yi;

			Y[i++] = Yi;  // Write to output
		}
		Z += order;	 // Next condition vector
	}

	return;
}

void NormalizeBA(double *ba, size_t nParam) {
	// Normalize filter parameters such that a[0] is 1.0.
	double a0 = ba[nParam];
	size_t i = 0, f = 2 * nParam;

	// Catch division by zero as error:
	if(a0 == 0.0) {
		mexErrMsgIdAndTxt("BadValueA", "1st element of A cannot be 0.");
	}

	while(i < f) {
		ba[i++] /= a0;
	}

	return;
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
	const mxArray *arr	 = prhs[0];
	int			   len	 = mxGetNumberOfElements(arr);
	double		  *input = mxGetPr(arr);
	mexPrintf("mex_iir prhs[%d], Array len = %d\n", 0, len);

	plhs[0]		   = mxCreateDoubleMatrix(1, len, mxREAL);
	double *Result = mxGetPr(plhs[0]);

	iir_t  iir			= iir_new(LENGTH, coef_a, coef_b, (FLOAT)*input);
	double TMPZ[LENGTH] = { 0 };

	double ba[LENGTH * 2];
	for(int i = 0; i < LENGTH; i++) {
		ba[i]		   = coef_b[i];
		ba[LENGTH + i] = coef_a[i];
		mexPrintf("b[%d]=%f, a[%d]=%f \n", i, ba[i], i, ba[LENGTH + i]);
	}
	NormalizeBA(ba, LENGTH);
	for(int i = 0; i < LENGTH; i++) {
		mexPrintf("Norm: b[%d]=%f, a[%d]=%f, Z[%d]=%f\n", i, ba[i], i, ba[LENGTH + i], i, TMPZ[i]);
	}

	CoreDoubleN(input, len, 1, &ba[LENGTH], ba, LENGTH, TMPZ, Result);

	// for(int i = 0; i < len; i++) {
	// 	// *(Result++) = (double)iir_step(iir, (FLOAT) * (input++));
	// 	*(Result++) = (double)test_butter((FLOAT) * (input++));
	// }

	iir_del(&iir);
}
