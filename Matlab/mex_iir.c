#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "mex.h"
#include "matrix.h"

#include "iir.h"

#define LENGTH (11)

static FLOAT coef_a[LENGTH] = {
	1,
	-6.78896078801031,
	21.1186057325602,
	-39.5466389186279,
	49.2807281548787,
	-42.6412048119369,
	25.9155696805515,
	-10.9133701016210,
	3.04505315550608,
	-0.507983530677825,
	0.0384514437152823,
};

static FLOAT coef_b[LENGTH] = {
	2.44156579882903e-07, 2.44156579882903e-06, 1.09870460947307e-05, 2.92987895859484e-05,
	5.12728817754097e-05, 6.15274581304916e-05, 5.12728817754097e-05, 2.92987895859484e-05,
	1.09870460947307e-05, 2.44156579882903e-06, 2.44156579882903e-07,
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

	mexPrintf("mex_iir: *X=%f,MX=%d,NX=%d,order=%d,*Z=%f,*Y=%d", *X, MX, NX, order, *Z, *Y);
	mexPrintf("\n  a = {");
	for(int k = 0; k < order + 1; k++) {
		mexPrintf("%.2f, ", a[k]);
	}
	mexPrintf("}\n  b = {");
	for(int k = 0; k < order + 1; k++) {
		mexPrintf("%.2f, ", b[k]);
	}
	mexPrintf("}\n");

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

	CoreDoubleN(input, len, 1, &ba[LENGTH], ba, LENGTH - 1, TMPZ, Result);

	// for(int i = 0; i < len; i++) {
	// 	// *(Result++) = (double)iir_step(iir, (FLOAT) * (input++));
	// 	*(Result++) = (double)test_butter((FLOAT) * (input++));
	// }

	iir_del(&iir);
}
