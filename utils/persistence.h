#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include "cv.h"
#include "stereocalibratoraux.h"

using namespace cv;

class Persistence
{
private:
    //! My private attributes
    struct pimp;
    pimp *self;

public:    
    Persistence();
    ~Persistence();
    int loadCalibration(int type = C_FILE_TYPE_YML);
    int saveCalibration(int type = C_FILE_TYPE_YML);
    void setCalibration(CvSize imageSize, CvMat *mx1, CvMat *my1, CvMat *mx2, CvMat *my2,
                         CvMat *M1, CvMat *M2, CvMat *D1, CvMat *D2,
                         CvMat *R, CvMat *T, CvMat *E, CvMat *F);
    void getCalibration(Size &imageSize, Mat& mx1, Mat& my1, Mat& mx2, Mat& my2,
                        Mat& M1, Mat& M2, Mat& D1, Mat& D2,
                        Mat& R, Mat& T, Mat& E, Mat& F);
    void getCalibration(CvSize &imageSize, CvMat* &mx1, CvMat* &my1, CvMat* &mx2, CvMat* &my2,
                        CvMat* &M1, CvMat* &M2, CvMat* &D1, CvMat* &D2,
                        CvMat* &R, CvMat* &T, CvMat* &E, CvMat* &F);
    void getCalibration(CvSize &imageSize, CvMat* &mx1, CvMat* &my1, CvMat* &mx2, CvMat* &my2);

    //! Gets OpenCV2
    Size getImageSize();
    Mat getMx1();
    Mat getMy1();
    Mat getMx2();
    Mat getMy2();
    Mat getM1();
    Mat getM2();
    Mat getD1();
    Mat getD2();
    Mat getR();
    Mat getT();
    Mat getE();
    Mat getF();
};

#endif // PERSISTENCE_H
