#include "stereoprocess.h"
#define RESULT_OK 0
#define RESULT_FAIL 1

//#include "cxmisc.h"
//#include "cvaux.h"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;

struct pimp {
    //! Mis privados aquí
    bool m_gotCalibration;
    void initAttribs();

    //! Matrices resultantes para rectificar imágenes
    Mat m_mx1, m_my1, m_mx2, m_my2;
    Size m_imageSize;
    //! Procesar un par de imágenes con la calibración adquirida previamente
    Mat m_imagesRectified[2];  //! Rectificación
    Mat m_imageDepth,m_imageDepthNormalized;  //! Mapa de disparidad
    Mat m_imageRectifiedPair; //! Par estereo
    Mat pairUnitedRectify();

    //! Depth map parameters
    int m_preFilterSize;
    int m_preFilterCap;
    int m_SADWindowSize;
    int m_minDisparity;
    int m_numberOfDisparities;
    int m_textureThreshold;
    int m_uniquenessRatio;
    int m_speckleWindowSize;
    int m_speckleRange;

    pimp(){
        m_preFilterSize=5;
        m_preFilterCap=63;
        m_SADWindowSize=9;
        m_minDisparity=-28;
        m_numberOfDisparities=64;
        m_textureThreshold=0;
        m_uniquenessRatio=0;
        m_speckleWindowSize=0;
        m_speckleRange=0;
    }
};

StereoProcess::StereoProcess()
{
    self = new pimp();
    self->m_gotCalibration = false;
}

void StereoProcess::setCalibration(Size imageSize, Mat mx1, Mat my1, Mat mx2, Mat my2)
{
    self->m_imageSize = imageSize;
    self->m_mx1 = mx1;
    self->m_my1 = my1;
    self->m_mx2 = mx2;
    self->m_my2 = my2;
    self->initAttribs();
}

bool StereoProcess::isCalibrated()
{
    return self->m_gotCalibration;
}

int StereoProcess::process(Mat imageSrcLeft, Mat imageSrcRight)
{
    int r = rectify(imageSrcLeft, imageSrcRight);
    if(r == RESULT_FAIL) return r;
    StereoBM BM(StereoBM::BASIC_PRESET, self->m_numberOfDisparities, self->m_SADWindowSize);
    BM.state->preFilterSize = self->m_preFilterSize;
    BM.state->preFilterCap = self->m_preFilterCap;
    //BM.state->SADWindowSize = self->m_SADWindowSize;
    BM.state->minDisparity = self->m_minDisparity;
    //BM.state->numberOfDisparities = self->m_numberOfDisparities;
    BM.state->textureThreshold = self->m_textureThreshold;
    BM.state->uniquenessRatio = self->m_uniquenessRatio;
    BM.state->speckleWindowSize = self->m_speckleWindowSize;
    BM.state->speckleRange = self->m_speckleRange;

    BM(self->m_imagesRectified[0], self->m_imagesRectified[1], self->m_imageDepth);    

    normalize(self->m_imageDepth, self->m_imageDepthNormalized, 0, 256, NORM_MINMAX, CV_8UC1);

//    Scalar tinta(100, 1, 1);
//    Mat A(self->m_imageDepthNormalized.rows, self->m_imageDepthNormalized.cols, CV_8UC3);
//    for (int i = 0; i < A.rows; ++i) {
//        const uchar* depthi = self->m_imageDepthNormalized.ptr<uchar>(i);
//        Vec3b* Ai = A.ptr<Vec3b>(i);
//        for (int j = 0; j < A.cols; ++j)
//            for (int k = 0; k < 3; ++k)
//                Ai[j][k] = depthi[j] * tinta[k];
//    }
//    imshow("A",A);

    return RESULT_OK;
}

int StereoProcess::rectify(Mat imageSrcLeft, Mat imageSrcRight)
{
    if(!self->m_gotCalibration) return RESULT_FAIL;
    remap(imageSrcLeft, self->m_imagesRectified[0], self->m_mx1, self->m_my1, INTER_LINEAR);
    remap(imageSrcRight, self->m_imagesRectified[1], self->m_mx2, self->m_my2, INTER_LINEAR);
    return RESULT_OK;
}

Mat StereoProcess::getImageRectified(int camera)
{
    return self->m_imagesRectified[camera];
}

Mat StereoProcess::getImageRectPair()
{
    return self->pairUnitedRectify();
}

Mat StereoProcess::getImageDepthNormalized()
{
    return self->m_imageDepthNormalized;
}

Mat StereoProcess::getImageDepth()
{
    return self->m_imageDepth;
}

void StereoProcess::setPreFilterSize(int value)
{
    self->m_preFilterSize = value;
}

void StereoProcess::setPreFilterCap(int value)
{
    self->m_preFilterCap = value;
}

void StereoProcess::setSADWindowSize(int value)
{
    self->m_SADWindowSize = value;
}

void StereoProcess::setMinDisparity(int value)
{
    self->m_minDisparity = value;
}

void StereoProcess::setNumberOfDisparities(int value)
{
    self->m_numberOfDisparities = value;
}

void StereoProcess::setTextureThreshold(int value)
{
    self->m_textureThreshold = value;
}

void StereoProcess::setUniquenessRatio(int value)
{
    self->m_uniquenessRatio = value;
}

void StereoProcess::setSpeckleWindowSize(int value)
{
    self->m_speckleWindowSize = value;
}

void StereoProcess::setSpeckleRange(int value)
{
    self->m_speckleRange = value;
}

StereoProcess::~StereoProcess()
{
    delete self;
}

void pimp::initAttribs()
{
    m_gotCalibration = true;
    m_imageRectifiedPair.create(m_imageSize.height, m_imageSize.width*2, CV_8UC3);
}

Mat pimp::pairUnitedRectify()
{
    Mat A,B;
    cvtColor(m_imagesRectified[0], A, CV_GRAY2BGR);
    cvtColor(m_imagesRectified[1], B, CV_GRAY2BGR);
    A.copyTo(m_imageRectifiedPair.colRange(0,m_imageSize.width));
    B.copyTo(m_imageRectifiedPair.colRange(m_imageSize.width,m_imageSize.width*2));

    //DIBUJAR LAS LINEAS
    for (int i = 0; i < m_imageSize.height; i += 16) {
        line(m_imageRectifiedPair, Point(0,i), Point(m_imageSize.width*2,i), CV_RGB((i%3)?0:255,((i+1)%3)?0:255,((i+2)%3)?0:255));
    }
    return m_imageRectifiedPair;
}

