#include "stereocapture.h"
#include <algorithm>

using namespace std;


StereoCapture::StereoCapture():m_ready(false),m_lowestFreq(-2)
{
    m_imageSize = Size(0,0);
    m_frequency[0] = -2;
    m_frequency[1] = -2;
}

StereoCapture::~StereoCapture()
{

}

int StereoCapture::setup(int leftCamera, int rightCamera)
{
    m_capture[0].open(leftCamera);
    m_capture[1].open(rightCamera);
    if(m_capture[0].isOpened() && m_capture[1].isOpened()){
        for (int i = 0; i < 2; ++i)
            m_capture[i] >> m_frame[i];

        //! El tamaño de la imágenes será el menor tomado por las cámaras
        Size size0 = m_frame[0].size(), size1 = m_frame[1].size();
        m_imageSize.width = min(size0.width, size1.width);
        m_imageSize.height = min(size0.height, size1.height);        

        for (int i = 0; i < 2; ++i) {
            m_capture[i].set(CV_CAP_PROP_FRAME_WIDTH, m_imageSize.width);
            m_capture[i].set(CV_CAP_PROP_FRAME_HEIGHT, m_imageSize.height);
        }

        m_frequency[0] = m_capture[0].get(CV_CAP_PROP_FPS);
        m_frequency[1] = m_capture[1].get(CV_CAP_PROP_FPS);

        m_lowestFreq = min(m_frequency[0], m_frequency[1]);

        emit sg_message(QString::fromUtf8("Tamaño de imágen: alto: %1 ancho: %2\n Frecuencia cámara izquierda: %3\n Frecuencia cámara derecha: %4\n").arg(m_imageSize.height).arg(m_imageSize.width).arg(m_frequency[0]).arg(m_frequency[1]));

        m_ready = true;
        return RESULT_OK;
    }
    m_ready = false;    
    if(m_capture[0].isOpened()) m_capture[0].release();
    if(m_capture[1].isOpened()) m_capture[1].release();
    return RESULT_FAIL;
}

int StereoCapture::close()
{
    m_ready = false;
    m_capture[0].release();
    m_capture[1].release();
    if(m_capture[0].isOpened() || m_capture[1].isOpened()) return RESULT_FAIL;
    return RESULT_OK;
}

bool StereoCapture::isReady()
{
    return m_ready;
}

//! \brief Using grab & retrieve minimize the time diference between two cameras
int StereoCapture::capture()
{
//    m_capture[0] >> m_frame[0];
//    m_capture[1] >> m_frame[1];
//    return (m_frame[0].data && m_frame[1].data) ? RESULT_OK : RESULT_FAIL;
    bool g1 = m_capture[0].grab();   
    bool g2 = m_capture[1].grab();
    if(!g1 || !g2) return RESULT_FAIL;
    m_capture[0].retrieve(m_frame[0]);
    m_capture[1].retrieve(m_frame[1]);
    return RESULT_OK;
}

Mat StereoCapture::getFrame(int camera)
{
    if(!isValidCamera(camera)) return Mat();
    return m_frame[camera];
}

Mat StereoCapture::getFrameGray(int camera)
{
    if(!isValidCamera(camera)) return Mat();
    cvtColor(m_frame[camera], m_frameGray[camera], CV_BGR2GRAY);
    return m_frameGray[camera];
}

double StereoCapture::getCameraFreq(int camera)
{
    if(!isValidCamera(camera)) return -2;
    if(m_frequency[camera] == -2) m_frequency[camera] =  m_capture[camera].get(CV_CAP_PROP_FPS);
    return m_frequency[camera];
}

double StereoCapture::getLowestFreq()
{
    if(m_lowestFreq != -2) return m_lowestFreq;
    m_frequency[0] = m_capture[0].get(CV_CAP_PROP_FPS);
    m_frequency[1] = m_capture[1].get(CV_CAP_PROP_FPS);
    m_lowestFreq = min(m_frequency[0],m_frequency[1]);
    return m_lowestFreq;
}

bool StereoCapture::isValidCamera(int camera)
{
    return (camera >= 0 || camera <= 1);
}
