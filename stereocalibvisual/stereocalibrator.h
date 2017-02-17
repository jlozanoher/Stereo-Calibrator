#ifndef STEREOCALIBRATOR_H
#define STEREOCALIBRATOR_H

#include <QObject>
#include <cv.h>
#include "../utils/stereocalibratoraux.h"

using namespace std;
using namespace cv;

class StereoCalibrator : public QObject
{
    Q_OBJECT
public:
    StereoCalibrator();
    ~StereoCalibrator();

    //! Calibration process
    void calibrationStart(Size imageSize, int pairCount, int cornersX, int cornersY, double squareSize);
    int calibrationAddPair(Mat imageLeft, Mat imageRight);
    int calibrationEnd();

    bool isCalibrationStarted(){ return m_calibrationStarted; }
    int getPairSuccess(){ return m_pairSuccess; }
    int getCalibratiorDone(){ return m_calibrationDone; }
    int getPairCount(){ return m_pairCount; }
    void setCalibrationDone(bool value){ m_calibrationDone = value; }

    //! Gets
    Size getImageSize(){ return m_imageSize; }
    double getMarkerSize(){ return m_squareSize; }
    Mat getM1(){  return m_M1; }
    Mat getM2(){  return m_M2; }
    Mat getD1(){  return m_D1; }
    Mat getD2(){  return m_D2; }
    Mat getR(){  return m_R; }
    Mat getT(){  return m_T; }
    Mat getE(){  return m_E; }
    Mat getF(){  return m_F; }
    Mat getMx1(){ return m_mx1; }
    Mat getMy1(){ return m_my1; }
    Mat getMx2(){ return m_mx2; }
    Mat getMy2(){ return m_my2; }
    Mat getP1(){ return m_P1; }
    Mat getP2(){ return m_P2; }
    Mat getR1(){ return m_R1; }
    Mat getR2(){ return m_R2; }

    //! Persistence
    int loadCalibration(QString path = C_CALIBRATION_FILE_YML, int type = C_FILE_TYPE_YML);
    int saveCalibration(QString path = C_CALIBRATION_FILE_YML, int type = C_FILE_TYPE_YML);

private:

    vector <vector <Point3f> > m_objectPoints;  //World coordinates
    vector <vector <Point2f> > m_imagePoints[2];  //Point position in pixels

    Mat m_M1, m_M2, m_D1, m_D2, m_R, m_T, m_E, m_F; //Calibration matrices

    Mat m_R1, m_R2, m_P1, m_P2, m_Q; //Rectification matrices

    Mat m_mx1, m_my1, m_mx2, m_my2;  //Maps for the rectification

    //! Utils
    Size m_imageSize;
    int m_cornersX, m_cornersY, m_cornersN; //! Amount of corners to find
    int m_pairCount;  //! Amount of pair of stereo images
    int m_pairSuccess;      //! Amount of good pairs
    double m_squareSize;  //! Size of the chessboard square

    bool m_calibrationStarted;
    bool m_calibrationDone;


signals:
    void sg_message(QString message);
public slots:

};

#endif // STEREOCALIBRATOR_H
