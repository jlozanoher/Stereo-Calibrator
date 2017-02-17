#include "stereocalibrator.h"

#include "opencv2/highgui/highgui.hpp"

StereoCalibrator::StereoCalibrator():
    m_calibrationStarted(false), m_calibrationDone(false)
{

}

StereoCalibrator::~StereoCalibrator()
{

}

void StereoCalibrator::calibrationStart(Size imageSize, int pairCount, int cornersX, int cornersY, double squareSize = 1)
{
    m_imageSize = imageSize;
    m_pairCount = pairCount;
    m_pairSuccess = 0;
    m_cornersX = cornersX;
    m_cornersY = cornersY;
    m_squareSize = squareSize;
    m_cornersN = cornersX * cornersY;
    m_calibrationStarted = true;
    m_calibrationDone = false;

    for (int i = 0; i < m_imagePoints[0].size(); ++i) {
        m_imagePoints[0][i].clear();
        m_imagePoints[1][i].clear();
        m_objectPoints[i].clear();
    }

    m_imagePoints[0].clear();
    m_imagePoints[1].clear();
    m_objectPoints.clear();

    m_M1.release();
    m_M2.release();
    m_D1.release();
    m_D2.release();

    m_R.release();
    m_T.release();
    m_E.release();
    m_F.release();

    m_R1.release();
    m_R2.release();
    m_P1.release();
    m_P2.release();
    m_Q.release();

    m_mx1.release();
    m_my1.release();
    m_mx2.release();
    m_my2.release();
}

int StereoCalibrator::calibrationAddPair(Mat imageLeft, Mat imageRight)
{
    if(!m_calibrationStarted){
        emit sg_message(QString::fromUtf8("La calibración no ha sido iniciada"));
        return RESULT_FAIL;
    }

    vector <Point2f> pointsTemp[2];
    Mat image[2] = {imageLeft, imageRight};
    int succcess = 0;
    for (int i = 0; i < 2; ++i) {
        //Find the cornes in the chessboard
        if(findChessboardCorners(image[i], Size(m_cornersX, m_cornersY), pointsTemp[i])){
            cornerSubPix(image[i], pointsTemp[i], Size(11,11), Size(-1,-1),
                         TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 30, 0.01));
            ++succcess;
            //drawChessboardCorners(image[i], Size(m_cornersX, m_cornersY),  pointsTemp[i], 1);
        }
    }

//    imshow("1",image[0]);
//    imshow("2",image[1]);

    //! If both images were successes, add points to the good points for calibrating
    if(2 == succcess){

        vector < Point3f > objectPointsTemp;
        for (int i = 0; i < m_cornersY; ++i) {
            for (int j = 0; j < m_cornersX; ++j) {
                objectPointsTemp.push_back( Point3f(i*m_squareSize, j*m_squareSize, 0.0f) );
            }
        }

        m_imagePoints[0].push_back(pointsTemp[0]);
        m_imagePoints[1].push_back(pointsTemp[1]);
        m_objectPoints.push_back(objectPointsTemp);
        ++m_pairSuccess;
        return RESULT_OK;
    }
    return RESULT_FAIL;
}

int StereoCalibrator::calibrationEnd()
{
    m_calibrationStarted = false;

    stereoCalibrate(m_objectPoints, m_imagePoints[0], m_imagePoints[1],
            m_M1, m_D1, m_M2, m_D2, m_imageSize, m_R, m_T, m_E, m_F,
            cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 100, 1e-5),
            CV_CALIB_FIX_ASPECT_RATIO + CV_CALIB_ZERO_TANGENT_DIST + CV_CALIB_SAME_FOCAL_LENGTH);

    stereoRectify(m_M1, m_D1, m_M2, m_D2, m_imageSize,
                  m_R, m_T, m_R1, m_R2, m_P1, m_P2, m_Q);


    initUndistortRectifyMap(m_M1, m_D1, m_R1, m_P1, m_imageSize, CV_32FC1, m_mx1, m_my1);
    initUndistortRectifyMap(m_M2, m_D2, m_R2, m_P2, m_imageSize, CV_32FC1, m_mx2, m_my2);

    m_calibrationDone = true;

    return RESULT_OK;
}

int StereoCalibrator::loadCalibration(QString path, int type)
{
    if(path.isEmpty()) return RESULT_FAIL;
    FileStorage fs;
    bool r;
    if(type == C_FILE_TYPE_XML)
        r = fs.open(QString(path).toStdString(), FileStorage::READ);
    else
        r = fs.open(QString(path).toStdString(), FileStorage::READ);

    if(!r) return RESULT_FAIL;

    FileNode node = fs[C_FRAME_SIZE];

    m_imageSize.width = (int)node[0];
    m_imageSize.height = (int)node[1];
    fs[C_INTRINSICS_LEFT] >> m_M1;
    fs[C_DISTORTION_LEFT] >> m_D1;
    fs[C_INTRINSICS_RIGHT] >> m_M2;
    fs[C_DISTORTION_RIGHT] >> m_D2;
    fs[C_ROTATION_MATRIX] >> m_R;
    fs[C_ROTATION_MATRIX1] >> m_R1;
    fs[C_ROTATION_MATRIX2] >> m_R2;
    fs[C_TRASLATION_MATRIX] >> m_T;
    fs[C_ESSENTIAL_MATRIX] >> m_E;
    fs[C_FUNDAMENTAL_MATRIX] >> m_F;
    fs[C_PROJECTION_MATRIX_LEFT] >> m_P1;
    fs[C_PROJECTION_MATRIX_RIGHT] >> m_P2;
    fs[C_DISPARITY_DEPTH_MATRIX] >> m_Q;
    fs[C_MX1] >> m_mx1;
    fs[C_MY1] >> m_my1;
    fs[C_MX2] >> m_mx2;
    fs[C_MY2] >> m_my2;

    m_calibrationDone = true;

    return RESULT_OK;
}

int StereoCalibrator::saveCalibration(QString path, int type)
{
    if(path.isEmpty()) return RESULT_FAIL;
    FileStorage fs;  
    bool r;
    if(type == C_FILE_TYPE_XML)
        r = fs.open(path.toStdString(), FileStorage::WRITE);
    else
        r = fs.open(path.toStdString(), FileStorage::WRITE);

    if(!r) return RESULT_FAIL;

    fs << C_FRAME_SIZE << "[" << m_imageSize.width << m_imageSize.height << "]";
    //! Matriz intrínseca camara izquierda
    fs << C_INTRINSICS_LEFT << m_M1;  //! Matriz intrínseca de la cámara izquierda
    fs << C_DISTORTION_LEFT << m_D1;  //! Coeficientes de distorsión de la cámara izquierda
    fs << C_INTRINSICS_RIGHT << m_M2;  //! Matriz intrínseca de la cámara derecha
    fs << C_DISTORTION_RIGHT << m_D2;  //! Coeficientes de distorsión de la cámara derecha
    fs << C_ROTATION_MATRIX << m_R;  //! Matriz de rotación de la cámara derecha con respecto a la izquierda
    fs << C_ROTATION_MATRIX1 << m_R1;  //! Matriz de rotación de la cámara izquierda
    fs << C_ROTATION_MATRIX2 << m_R2;  //! Matriz de rotación de la cámara derecha
    fs << C_TRASLATION_MATRIX << m_T;  //! Matriz de traslación de la cámara derecha con respecto a la izquierda
    fs << C_ESSENTIAL_MATRIX << m_E;  //! Matriz esencial
    fs << C_FUNDAMENTAL_MATRIX << m_F;  //! Matriz fundamental
    fs << C_PROJECTION_MATRIX_LEFT << m_P1;  //! Nueva matriz de proyeccion de la camara izquierda después de la rectificación
    fs << C_PROJECTION_MATRIX_RIGHT << m_P2;  //! Nueva matriz de proyeccion de la camara derecha después de la rectificación
    fs << C_DISPARITY_DEPTH_MATRIX << m_Q;  //!  4 × 4  matriz para mapear la disparidad-profundidad
    //! Mapas de interpolación para la rectificación
    fs << C_MX1 << m_mx1;
    fs << C_MY1 << m_my1;
    fs << C_MX2 << m_mx2;
    fs << C_MY2 << m_my2;

    return RESULT_OK;
}
