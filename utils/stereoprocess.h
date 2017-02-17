#ifndef STEREOPROCESS_H
#define STEREOPROCESS_H

#include <QObject>
#include "cv.h"
#include "opencv2/core/core.hpp"
/*!
 * \brief The StereoProcess class process a pair of stereo images *
 */

using namespace cv;

struct pimp;

class StereoProcess : public QObject
{
    Q_OBJECT
private:
    //! \note Design c++ pattern forward declaration of pimp, for the privates attributes

    pimp *self;
public:
    StereoProcess();
    ~StereoProcess();
    void setCalibration(Size imageSize, Mat mx1, Mat my1, Mat mx2, Mat my2);

    bool isCalibrated();

    int process(Mat imageSrcLeft, Mat imageSrcRight);

    int rectify(Mat imageSrcLeft, Mat imageSrcRight);

    Mat getImageRectified(int camera);
    Mat getImageRectPair();
    Mat getImageDepthNormalized();
    Mat getImageDepth();

    //! Depth map params

public slots:
    void setPreFilterSize(int value);
    void setPreFilterCap(int value);
    void setSADWindowSize(int value);
    void setMinDisparity(int value);
    void setNumberOfDisparities(int value);
    void setTextureThreshold(int value);
    void setUniquenessRatio(int value);
    void setSpeckleWindowSize(int value);
    void setSpeckleRange(int value);

};

#endif // STEREOPROCESS_H
