# Stereo-Calibrator
Stereoscopic Calibrator: A visual tool for calibrating the stereoscopic system. A chessboard is needed to perform the calibration. The number of horizontal and vertical boxes, the size of the square in any unit of measurement (meters, cm, mm, etc.) are specified, this unit will be used later in any measurement. Once calibration begins the system will take n pairs of images with the board visible for both cameras. Once all the pairs are taken, the system is calibrated and the calibration is stored in a file called "calibration.yml" or "calibration.xml" depending on the chosen format. This file is necessary for the tracker.

# Dependencies
- CMake
- Qt 4.8
- OpenCV