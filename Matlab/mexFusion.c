// #include "Fusion.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "mex.h"

#define SAMPLE_PERIOD (0.01f)  // replace this with actual sample period

struct fusion_input {
	double ts;
	double dt;
	double ax;
	double ay;
	double az;
	double gx;
	double gy;
	double gz;
};
typedef struct fusion_input *fusion_input_t;

int matlab_fusion_struct_to_C(fusion_input_t dst, const mxArray *arr) {
	const char	**fnames; /* pointers to field names */
	const mwSize *dims;
	mxArray		 *fout;
	char		 *pdata = NULL;
	mxClassID	 *classIDflags;
	mwSize		  ndim;

	const mxArray *root_arr = arr;
	/* get input arguments */
	int root_field_number	= mxGetNumberOfFields(root_arr);
	int root_element_number = mxGetNumberOfElements(root_arr);

	mexPrintf("root_field_number = %d, root_element_number = %d\n",
			  root_field_number,
			  root_element_number);

	for(int ifield = 0; ifield < root_field_number; ifield++) {
		mxArray *field = mxGetFieldByNumber(root_arr, 0, ifield);
		mexPrintf("root_field[%d] have %d elements\n", ifield, mxGetNumberOfElements(field));
	}

	/* allocate memory for storing classIDflags */
	classIDflags = mxCalloc(root_field_number, sizeof(mxClassID));

	// /* check empty field, proper data type, and data type consistency;
	//  * and get classID for each field. */
	// for(int ifield = 0; ifield < root_field_number; ifield++) {
	// 	for(mwIndex jstruct = 0; jstruct < root_element_number; jstruct++) {
	// 		mxArray *tmp = mxGetFieldByNumber(arr, jstruct, ifield);
	// 		if(tmp == NULL) {
	// 			mexPrintf("%s%d\t%s%d\n", "FIELD: ", ifield + 1, "STRUCT INDEX :", jstruct + 1);
	// 			mexErrMsgIdAndTxt("mexFusion:fieldEmpty", "Above field is empty!");
	// 		}
	// 		if(jstruct == 0) {
	// 			if((!mxIsChar(tmp) && !mxIsNumeric(tmp)) || mxIsSparse(tmp)) {
	// 				mexPrintf("%s%d\t%s%d\n", "FIELD: ", ifield + 1, "STRUCT INDEX :", jstruct + 1);
	// 				mexErrMsgIdAndTxt(
	// 					"mexFusion:invalidField",
	// 					"Above field must have either string or numeric non-sparse data.");
	// 			}
	// 			classIDflags[ifield] = mxGetClassID(tmp);
	// 		} else {
	// 			if(mxGetClassID(tmp) != classIDflags[ifield]) {
	// 				mexPrintf("%s%d\t%s%d\n", "FIELD: ", ifield + 1, "STRUCT INDEX :", jstruct + 1);
	// 				mexErrMsgIdAndTxt("mexFusion:invalidFieldType",
	// 								  "Inconsistent data type in above field!");
	// 			} else if(!mxIsChar(tmp)
	// 					  && ((mxIsComplex(tmp) || mxGetNumberOfElements(tmp) != 1))) {
	// 				mexPrintf("%s%d\t%s%d\n", "FIELD: ", ifield + 1, "STRUCT INDEX :", jstruct + 1);
	// 				mexErrMsgIdAndTxt("mexFusion:fieldNotRealScalar",
	// 								  "Numeric data in above field must be scalar and noncomplex!");
	// 			}
	// 		}
	// 	}
	// }

	mxArray *field				  = mxGetFieldByNumber(root_arr, 0, 0);
	int		 field_element_number = mxGetNumberOfElements(field);
	mexPrintf("root_field[%d] have %d elements\n", 0, mxGetNumberOfElements(field));
	for(mwIndex element_idx = 0; element_idx < field_element_number; element_idx++) {
		for(int ifield = 0; ifield < root_field_number; ifield++) {
			mxArray *tmp	 = mxGetFieldByNumber(field, element_idx, ifield);
			double	*realPtr = mxGetPr(tmp);
			if(realPtr == NULL) {
				mexPrintf("Failed to get value for element[%d] field[%d]\n", element_idx, ifield);
				continue;
			}
			const char *field_name = mxGetFieldNameByNumber(root_arr, ifield);
			mexPrintf("#%d field[%d:\"%s\"] = %f\n", element_idx, ifield, field_name, *realPtr);
			switch(ifield) {
			case 0: dst->ts = *realPtr; break;
			case 1: dst->dt = *realPtr; break;
			case 2: dst->ax = *realPtr; break;
			case 3: dst->ay = *realPtr; break;
			case 4: dst->az = *realPtr; break;
			case 5: dst->gx = *realPtr; break;
			case 6: dst->gy = *realPtr; break;
			case 7: dst->gz = *realPtr; break;
			default: break;
			}
		}
	}

	mxFree(classIDflags);
	return 0;
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
	// mexPrintf("%d input argument(s).\n", nrhs);
	// mexPrintf("%d output argument(s).\n", nlhs);

	for(int i = 0; i < nrhs; i++) {
		struct fusion_input fusion_input;

		// mexPrintf("Process prhs[%d]\n", i);
		const mxArray *arr = prhs[i];
		if(!mxIsStruct(arr)) {
			mexPrintf("Skip prhs[%d]\n", i);
			continue;
		}

		if(matlab_fusion_struct_to_C(&fusion_input, arr) != 0) {
			mexPrintf("Failed to convert prhs[%d]\n", i);
			continue;
		}

		mexPrintf(" t=%f,dt=%f,acc:{%f,%f,%f},gyro:{%f,%f,%f}\n",
				  fusion_input.ts,
				  fusion_input.dt,
				  fusion_input.ax,
				  fusion_input.ay,
				  fusion_input.az,
				  fusion_input.gx,
				  fusion_input.gy,
				  fusion_input.gz);
	}

	// FusionAhrs ahrs;
	// FusionAhrsInitialise(&ahrs);

	// while (true) { // this loop should repeat each time new gyroscope data is available
	//     const FusionVector gyroscope = {0.0f, 0.0f, 0.0f}; // replace this with actual gyroscope
	//     data in degrees/s const FusionVector accelerometer = {0.0f, 0.0f, 1.0f}; // replace this
	//     with actual accelerometer data in g

	//     FusionAhrsUpdateNoMagnetometer(&ahrs, gyroscope, accelerometer, SAMPLE_PERIOD);

	//     const FusionEuler euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));

	//     printf("Roll %0.1f, Pitch %0.1f, Yaw %0.1f\n", euler.angle.roll, euler.angle.pitch,
	//     euler.angle.yaw);
	// }
}
