#include "Fusion.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "mex.h"
#include "matrix.h"

#define SAMPLE_PERIOD (0.01f)  // replace this with actual sample period

#if MX_HAS_INTERLEAVED_COMPLEX
#define GET_POINTER(_arr) mxGetDoubles((_arr))
#else
#define GET_POINTER(_arr) mxGetPr((_arr))
#endif

struct fusion_input {
	size_t	len;
	double *ts;
	double *dt;
	double *ax;
	double *ay;
	double *az;
	double *gx;
	double *gy;
	double *gz;
};
typedef struct fusion_input *fusion_input_t;

struct fusion_output {
	size_t	  len;
	mxDouble *ts;
	mxDouble *dt;
	mxDouble *qw;
	mxDouble *qx;
	mxDouble *qy;
	mxDouble *qz;
	mxDouble *roll;
	mxDouble *pitch;
	mxDouble *yaw;
	mxDouble *linear_acc_x;
	mxDouble *linear_acc_y;
	mxDouble *linear_acc_z;
	mxDouble *earth_acc_x;
	mxDouble *earth_acc_y;
	mxDouble *earth_acc_z;
};
typedef struct fusion_output *fusion_output_t;

int matlab_fusion_input_struct_to_C(fusion_input_t dst, const mxArray *matlab_fusion_struct) {
	const char		  **fnames; /* pointers to field names */
	const mwSize	   *dims;
	mxArray			   *fout;
	char			   *pdata = NULL;
	mxClassID		   *classIDflags;
	mwSize				ndim;
	struct fusion_input tmp_fusion_input;

	/* get input arguments */
	int matlab_fusion_struct_field_number = mxGetNumberOfFields(matlab_fusion_struct);
	mexPrintf("matlab_fusion_struct_field_number = %d\n", matlab_fusion_struct_field_number);
	// Get field number and verify
	mxArray *first_field = mxGetFieldByNumber(matlab_fusion_struct, 0, 0);
	tmp_fusion_input.len = mxGetNumberOfElements(first_field);
	mexPrintf("Expected element number: %d\n", tmp_fusion_input.len);
	bool structure_matched = true;
	if(matlab_fusion_struct_field_number != 8) {
		structure_matched = false;
	}
	// Extract fields
	for(int ifield = 0; ifield < matlab_fusion_struct_field_number; ifield++) {
		mxArray	   *field		   = mxGetFieldByNumber(matlab_fusion_struct, 0, ifield);
		const char *field_name	   = mxGetFieldNameByNumber(matlab_fusion_struct, ifield);
		size_t		element_number = mxGetNumberOfElements(field);
		// Verify element number
		if(tmp_fusion_input.len != element_number) {
			mexPrintf("Element number of matlab_fusion_struct.%s not match, %d/%d\n",
					  field_name,
					  element_number,
					  tmp_fusion_input.len);
			structure_matched = false;
			break;
		}
		// Verify field data type
		if(!mxIsDouble(field)) {
			mexPrintf("matlab_fusion_struct.%s is not double\n", field_name);
			structure_matched = false;
			break;
		}
		// Get field data pointers
		double *value = GET_POINTER(field);
		mxAssert(value, "Null pointer error");

		if(strcmp(field_name, "ts") == 0) {
			tmp_fusion_input.ts = value;
		} else if(strcmp(field_name, "dt") == 0) {
			tmp_fusion_input.dt = value;
		} else if(strcmp(field_name, "ax") == 0) {
			tmp_fusion_input.ax = value;
		} else if(strcmp(field_name, "ay") == 0) {
			tmp_fusion_input.ay = value;
		} else if(strcmp(field_name, "az") == 0) {
			tmp_fusion_input.az = value;
		} else if(strcmp(field_name, "gx") == 0) {
			tmp_fusion_input.gx = value;
		} else if(strcmp(field_name, "gy") == 0) {
			tmp_fusion_input.gy = value;
		} else if(strcmp(field_name, "gz") == 0) {
			tmp_fusion_input.gz = value;
		} else {
			mexPrintf("matlab_fusion_struct.%s is not a fusion struct field\n", field_name);
			structure_matched = false;
			break;
		}
	}
	// Copy data pointers to destination
	if(structure_matched) {
		mexPrintf("Fusion structure matched\n");
		*dst = tmp_fusion_input;
	}
	return 0;
}

int C_fusion_output_struct_to_matlab(mxArray *output, fusion_output_t src) {
	mxArray *output_field_ts;
	mxArray *output_field_dt;
	mxArray *output_field_qw;
	mxArray *output_field_qx;
	mxArray *output_field_qy;
	mxArray *output_field_qz;
	mxArray *output_field_roll;
	mxArray *output_field_pitch;
	mxArray *output_field_yaw;
	mxArray *output_field_linear_acc_x;
	mxArray *output_field_linear_acc_y;
	mxArray *output_field_linear_acc_z;
	mxArray *output_field_earth_acc_x;
	mxArray *output_field_earth_acc_y;
	mxArray *output_field_earth_acc_z;
	// Create mxArray data structures to hold the data
	// // to be assigned for the structure.
	// mystring = mxCreateString("This is my char");
	// mydouble = mxCreateDoubleMatrix(1, 100, mxREAL);
	// dblptr	 = mxGetPr(mydouble);
	// for(i = 0; i < 100; i++)
	// 	dblptr[i] = (double)i;

	// // Assign field names
	// fieldnames[0] = (char *)mxMalloc(20);
	// fieldnames[1] = (char *)mxMalloc(20);

	return 0;
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
	mxArray				*ts;
	mxArray				*dt;
	mxArray				*qw;
	mxArray				*qx;
	mxArray				*qy;
	mxArray				*qz;
	mxArray				*roll;
	mxArray				*pitch;
	mxArray				*yaw;
	mxArray				*linear_acc_x;
	mxArray				*linear_acc_y;
	mxArray				*linear_acc_z;
	mxArray				*earth_acc_x;
	mxArray				*earth_acc_y;
	mxArray				*earth_acc_z;
	struct fusion_input	 fusion_input;
	struct fusion_output fusion_output = { 0 };

	// mexPrintf("%d input argument(s).\n", nrhs);
	// mexPrintf("%d output argument(s).\n", nlhs);

	for(int i = 0; i < nrhs; i++) {
		mexPrintf("Process prhs[%d]\n", i);
		const mxArray *arr = prhs[i];
		if(!mxIsStruct(arr)) {
			mexPrintf("Skip prhs[%d]\n", i);
			continue;
		}

		if(matlab_fusion_input_struct_to_C(&fusion_input, arr) != 0) {
			mexPrintf("Failed to convert prhs[%d]\n", i);
			continue;
		}

		if(nlhs >= 1) {
			const char *field_names[15] = {
				[0]	 = "ts",
				[1]	 = "dt",
				[2]	 = "qw",
				[3]	 = "qx",
				[4]	 = "qy",
				[5]	 = "qz",
				[6]	 = "roll",
				[7]	 = "pitch",
				[8]	 = "yaw",
				[9]	 = "linear_acc_x",
				[10] = "linear_acc_y",
				[11] = "linear_acc_z",
				[12] = "earth_acc_x",
				[13] = "earth_acc_y",
				[14] = "earth_acc_z",
			};
			plhs[0]			  = mxCreateStructMatrix(1, 1, 15, field_names);
			fusion_output.len = fusion_input.len;

			ts			 = mxCreateNumericMatrix(fusion_output.len, 1, mxDOUBLE_CLASS, mxREAL);
			dt			 = mxCreateNumericMatrix(fusion_output.len, 1, mxDOUBLE_CLASS, mxREAL);
			qw			 = mxCreateNumericMatrix(fusion_output.len, 1, mxDOUBLE_CLASS, mxREAL);
			qx			 = mxCreateNumericMatrix(fusion_output.len, 1, mxDOUBLE_CLASS, mxREAL);
			qy			 = mxCreateNumericMatrix(fusion_output.len, 1, mxDOUBLE_CLASS, mxREAL);
			qz			 = mxCreateNumericMatrix(fusion_output.len, 1, mxDOUBLE_CLASS, mxREAL);
			roll		 = mxCreateNumericMatrix(fusion_output.len, 1, mxDOUBLE_CLASS, mxREAL);
			pitch		 = mxCreateNumericMatrix(fusion_output.len, 1, mxDOUBLE_CLASS, mxREAL);
			yaw			 = mxCreateNumericMatrix(fusion_output.len, 1, mxDOUBLE_CLASS, mxREAL);
			linear_acc_x = mxCreateNumericMatrix(fusion_output.len, 1, mxDOUBLE_CLASS, mxREAL);
			linear_acc_y = mxCreateNumericMatrix(fusion_output.len, 1, mxDOUBLE_CLASS, mxREAL);
			linear_acc_z = mxCreateNumericMatrix(fusion_output.len, 1, mxDOUBLE_CLASS, mxREAL);
			earth_acc_x	 = mxCreateNumericMatrix(fusion_output.len, 1, mxDOUBLE_CLASS, mxREAL);
			earth_acc_y	 = mxCreateNumericMatrix(fusion_output.len, 1, mxDOUBLE_CLASS, mxREAL);
			earth_acc_z	 = mxCreateNumericMatrix(fusion_output.len, 1, mxDOUBLE_CLASS, mxREAL);

			fusion_output.ts		   = GET_POINTER(ts);
			fusion_output.dt		   = GET_POINTER(dt);
			fusion_output.qw		   = GET_POINTER(qw);
			fusion_output.qx		   = GET_POINTER(qx);
			fusion_output.qy		   = GET_POINTER(qy);
			fusion_output.qz		   = GET_POINTER(qz);
			fusion_output.roll		   = GET_POINTER(roll);
			fusion_output.pitch		   = GET_POINTER(pitch);
			fusion_output.yaw		   = GET_POINTER(yaw);
			fusion_output.linear_acc_x = GET_POINTER(linear_acc_x);
			fusion_output.linear_acc_y = GET_POINTER(linear_acc_y);
			fusion_output.linear_acc_z = GET_POINTER(linear_acc_z);
			fusion_output.earth_acc_x  = GET_POINTER(earth_acc_x);
			fusion_output.earth_acc_y  = GET_POINTER(earth_acc_y);
			fusion_output.earth_acc_z  = GET_POINTER(earth_acc_z);

			mxAssert(fusion_output.ts, "Null pointer error");
			mxAssert(fusion_output.dt, "Null pointer error");
			mxAssert(fusion_output.qw, "Null pointer error");
			mxAssert(fusion_output.qx, "Null pointer error");
			mxAssert(fusion_output.qy, "Null pointer error");
			mxAssert(fusion_output.qz, "Null pointer error");
			mxAssert(fusion_output.roll, "Null pointer error");
			mxAssert(fusion_output.pitch, "Null pointer error");
			mxAssert(fusion_output.yaw, "Null pointer error");
			mxAssert(fusion_output.linear_acc_x, "Null pointer error");
			mxAssert(fusion_output.linear_acc_y, "Null pointer error");
			mxAssert(fusion_output.linear_acc_z, "Null pointer error");
			mxAssert(fusion_output.earth_acc_x, "Null pointer error");
			mxAssert(fusion_output.earth_acc_y, "Null pointer error");
			mxAssert(fusion_output.earth_acc_z, "Null pointer error");

			FusionAhrs ahrs;
			FusionAhrsInitialise(&ahrs);
			for(int i = 0; i < fusion_input.len; i++) {
				fusion_output.ts[i] = fusion_input.ts[i];
				fusion_output.dt[i] = fusion_input.dt[i];
				// Gyroscope data in degrees/s
				const FusionVector gyroscope = { fusion_input.gx[i],
												 fusion_input.gy[i],
												 fusion_input.gz[i] };
				// Accelerometer data in g
				const FusionVector accelerometer = { fusion_input.ax[i],
													 fusion_input.ay[i],
													 fusion_input.az[i] };

				FusionAhrsUpdateNoMagnetometer(&ahrs, gyroscope, accelerometer, fusion_input.dt[i]);

				const FusionQuaternion q		  = FusionAhrsGetQuaternion(&ahrs);
				const FusionEuler	   euler	  = FusionQuaternionToEuler(q);
				const FusionVector	   linear_acc = FusionAhrsGetLinearAcceleration(&ahrs);
				const FusionVector	   earth_acc  = FusionAhrsGetEarthAcceleration(&ahrs);

				fusion_output.qw[i] = q.array[0];
				fusion_output.qx[i] = q.array[1];
				fusion_output.qy[i] = q.array[2];
				fusion_output.qz[i] = q.array[3];

				fusion_output.roll[i]  = euler.angle.roll;
				fusion_output.pitch[i] = euler.angle.pitch;
				fusion_output.yaw[i]   = euler.angle.yaw;

				fusion_output.linear_acc_x[i] = linear_acc.axis.x;
				fusion_output.linear_acc_y[i] = linear_acc.axis.y;
				fusion_output.linear_acc_z[i] = linear_acc.axis.z;

				fusion_output.earth_acc_x[i] = earth_acc.axis.x;
				fusion_output.earth_acc_y[i] = earth_acc.axis.y;
				fusion_output.earth_acc_z[i] = earth_acc.axis.z;

				// mexPrintf("Roll %0.1f, Pitch %0.1f, Yaw %0.1f\n",
				// 	   euler.angle.roll,
				// 	   euler.angle.pitch,
				// 	   euler.angle.yaw);
				// mexPrintf(" t=%.3f,dt=%.3f,acc:{%f,%f,%f},gyro:{%f,%f,%f}\n",
				// 		  fusion_input.ts[i],
				// 		  fusion_input.dt[i],
				// 		  fusion_input.ax[i],
				// 		  fusion_input.ay[i],
				// 		  fusion_input.az[i],
				// 		  fusion_input.gx[i],
				// 		  fusion_input.gy[i],
				// 		  fusion_input.gz[i]);
			}

			mxSetFieldByNumber(plhs[0], 0, 0, ts);
			mxSetFieldByNumber(plhs[0], 0, 1, dt);
			mxSetFieldByNumber(plhs[0], 0, 2, qw);
			mxSetFieldByNumber(plhs[0], 0, 3, qx);
			mxSetFieldByNumber(plhs[0], 0, 4, qy);
			mxSetFieldByNumber(plhs[0], 0, 5, qz);
			mxSetFieldByNumber(plhs[0], 0, 6, roll);
			mxSetFieldByNumber(plhs[0], 0, 7, pitch);
			mxSetFieldByNumber(plhs[0], 0, 8, yaw);
			mxSetFieldByNumber(plhs[0], 0, 9, linear_acc_x);
			mxSetFieldByNumber(plhs[0], 0, 10, linear_acc_y);
			mxSetFieldByNumber(plhs[0], 0, 11, linear_acc_z);
			mxSetFieldByNumber(plhs[0], 0, 12, earth_acc_x);
			mxSetFieldByNumber(plhs[0], 0, 13, earth_acc_y);
			mxSetFieldByNumber(plhs[0], 0, 14, earth_acc_z);

			C_fusion_output_struct_to_matlab(plhs[0], &fusion_output);
		}
	}
}
