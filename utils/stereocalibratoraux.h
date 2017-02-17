#ifndef STEREOCALIBRATORAUX_H
#define STEREOCALIBRATORAUX_H

#define RESULT_OK 0
#define RESULT_FAIL 1
#define CAMERA_LEFT 0
#define CAMERA_RIGHT 1
#define CAMERA_INSIDE 0

//Mostrar imágenes
#define SHOW_ONLY_CAMERA_IMAGES 0
#define SHOW_CAMERA_GRAY 1
#define SHOW_RECTIFICATION 2
#define SHOW_DEPTH_MAP 4

//Calibration
#define C_FILE_TYPE_XML 1
#define C_FILE_TYPE_YML 2
#define C_CALIBRATION_FILE "../calibration"
#define C_CALIBRATION_FILE_XML "../calibration.xml"
#define C_CALIBRATION_FILE_YML "../calibration.yml"
#define C_FRAME_SIZE "frame_size"
#define C_INTRINSICS_LEFT "intrinsics_left"
#define C_DISTORTION_LEFT "distortion_left"
#define C_INTRINSICS_RIGHT "intrinsics_right"
#define C_DISTORTION_RIGHT "distortion_right"
#define C_ROTATION_MATRIX "rotation_matrix"
#define C_ROTATION_MATRIX1 "rotation_matrix1"
#define C_ROTATION_MATRIX2 "rotation_matrix2"
#define C_TRASLATION_MATRIX "traslation_matrix"
#define C_ESSENTIAL_MATRIX "essential_matrix"
#define C_FUNDAMENTAL_MATRIX "fundamental_matrix"
#define C_PROJECTION_MATRIX_LEFT "P1"
#define C_PROJECTION_MATRIX_RIGHT "P2"
#define C_DISPARITY_DEPTH_MATRIX "Q"


#define C_MX1 "mx1"
#define C_MY1 "my1"
#define C_MX2 "mx2"
#define C_MY2 "my2"


#endif // STEREOCALIBRATORAUX_H
