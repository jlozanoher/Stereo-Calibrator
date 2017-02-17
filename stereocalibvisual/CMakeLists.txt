project(StereoCalibVisual)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_BUILD_TYPE DEBUG)

#Use QT4.8
find_package(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

#Use OpenCV
find_package(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OPEN_CV_INCLUDE_DIR})

#Sources and headers
set(StereoCalibVisual_SRCS
    main.cpp
    mainwindow.cpp    
    stereocalibrator.cpp
    ../utils/stereoprocess.cpp
    ../utils/stereocalibratoraux.h
    ../utils/stereocapture.cpp
    resources.qrc
)

#UIS
set(StereoCalibVisual_UIS
    mainwindow.ui
)

#aux_source_directory(. SRC_LIST)

#Ejecutable
add_executable(${PROJECT_NAME} ${StereoCalibVisual_SRCS} ${StereoCalibVisual_UIS})

#Dependencias del Qt y OpenCV
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Qt4::QtCore Qt4::QtGui ${OpenCV_LIBS})

