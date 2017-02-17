#ifndef STEREOCAPTURE_H
#define STEREOCAPTURE_H

#include "cv.h"
#include "cvaux.h"
#include "highgui.h"
#include <QObject>
#include "stereocalibratoraux.h"

using namespace cv;

class StereoCapture : public QObject
{
    Q_OBJECT
public:
    StereoCapture();
    ~StereoCapture();
     //! Inicializa parametros de las cámaras
    int setup(int leftCamera, int rightCamera);

    //! Cierra la comunicación de las cámaras y retorna RESULT_OK si no hay problemas
    int close();

    bool isReady();

     //! Captura el frame correspondiente de cada cámara
    int capture();

    Mat getFrame(int camera);

    //! Retorna el frame en escala de grises de una camara
    Mat getFrameGray(int camera);

    Size getImageSize(){return m_imageSize;}

    //! Retorna la frecuencia de captura de la cámara, -1 en caso de que el índice entrado sea incorrecto
    double getCameraFreq(int camera);

    //! Retorna la menor frecuencia , que es la usada
    double getLowestFreq();

private:
    bool m_ready;     //! True si se cargaron correctamente las cámaras

    VideoCapture m_capture[2]; //! Capturas para las dos cámaras
    Size m_imageSize;  //! Tamaño de las imágenes
    Mat m_frame[2];  //! Frame actual
    Mat m_frameGray[2];    //! Frame actual en escala de grises

    double m_frequency[2], m_lowestFreq;

    bool isValidCamera(int camera);


signals:
    void sg_message(QString message);
};

#endif // STEREOCAPTURE_H
