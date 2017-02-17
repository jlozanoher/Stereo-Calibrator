#include "persistence.h"
#include <QDebug>

struct Persistence::pimp {
    CvSize m_imageSize;
    //! ELementos para la calibración estereo
    CvMat *m_M1,*m_M2,*m_D1,*m_D2,*m_R,*m_T,*m_E,*m_F;
    //! Matrices resultantes para rectificar imágenes
    CvMat *m_mx1, *m_my1, *m_mx2, *m_my2;

    Size n_imageSize;
    Mat n_M1,n_M2,n_D1,n_D2,n_R,n_T,n_E,n_F;
    Mat n_mx1, n_my1, n_mx2, n_my2;

    int readCalibration(int type){


        FileStorage fs2(C_CALIBRATION_FILE_YML, FileStorage::READ);
        FileNode node = fs2[C_FRAME_SIZE];
        n_imageSize.width = (int)node[0];
        n_imageSize.height = (int)node[1];
        fs2[C_INTRINSICS_LEFT] >> n_M1;
        fs2[C_DISTORTION_LEFT] >> n_D1;
        fs2[C_INTRINSICS_RIGHT] >> n_M2;
        fs2[C_DISTORTION_RIGHT] >> n_D2;
        fs2[C_ROTATION_MATRIX] >> n_R;
        fs2[C_TRASLATION_MATRIX] >> n_T;
        fs2[C_ESSENTIAL_MATRIX] >> n_E;
        fs2[C_FUNDAMENTAL_MATRIX] >> n_F;
        fs2[C_MX1] >> n_mx1;
        fs2[C_MY1] >> n_my1;
        fs2[C_MX2] >> n_mx2;
        fs2[C_MY2] >> n_my2;


        //! Se debe leer el fichero teniendo en cuenta el orden con que fue escrito
        //! Manejador del fichero
        CvFileStorage *fs;
        if(type == C_FILE_TYPE_XML)
            fs = cvOpenFileStorage(C_CALIBRATION_FILE_XML, 0, CV_STORAGE_READ);
        else
            fs = cvOpenFileStorage(C_CALIBRATION_FILE_YML, 0, CV_STORAGE_READ);

        if(fs == 0) return RESULT_FAIL;

        //! Tamaño de la imagen
        CvSeq *s = cvGetFileNodeByName(fs, 0, C_FRAME_SIZE)->data.seq;
        m_imageSize.width = cvReadInt((CvFileNode*)cvGetSeqElem(s,0));
        m_imageSize.height = cvReadInt((CvFileNode*)cvGetSeqElem(s,1));

        m_M1 = (CvMat*) cvReadByName(fs, 0, C_INTRINSICS_LEFT);
        m_D1 = (CvMat*) cvReadByName(fs, 0, C_DISTORTION_LEFT);
        m_M2 = (CvMat*) cvReadByName(fs, 0, C_INTRINSICS_RIGHT);
        m_D2 = (CvMat*) cvReadByName(fs, 0, C_DISTORTION_RIGHT);
        m_R = (CvMat*) cvReadByName(fs, 0, C_ROTATION_MATRIX);
        m_T = (CvMat*) cvReadByName(fs, 0, C_TRASLATION_MATRIX);
        m_E = (CvMat*) cvReadByName(fs, 0, C_ESSENTIAL_MATRIX);
        m_F = (CvMat*) cvReadByName(fs, 0, C_FUNDAMENTAL_MATRIX);
        m_mx1 = (CvMat*) cvReadByName(fs, 0, C_MX1);
        m_my1 = (CvMat*) cvReadByName(fs, 0, C_MY1);
        m_mx2 = (CvMat*) cvReadByName(fs, 0, C_MX2);
        m_my2 = (CvMat*) cvReadByName(fs, 0, C_MY2);

        cvReleaseFileStorage(&fs);
        return RESULT_OK;
    }

    int writeCalibration(int type)
    {
        //! Manejador para la escritura del fichero
        CvFileStorage *fs;
        if(type == C_FILE_TYPE_XML)
            fs = cvOpenFileStorage(C_CALIBRATION_FILE_XML, 0, CV_STORAGE_WRITE);
        else
            fs = cvOpenFileStorage(C_CALIBRATION_FILE_YML, 0, CV_STORAGE_WRITE);

        if(fs == 0) return RESULT_FAIL;

        //! Escribiendo el tamaño de imagen
        cvStartWriteStruct(fs, C_FRAME_SIZE, CV_NODE_SEQ);
        cvWriteInt(fs, 0, m_imageSize.width);
        cvWriteInt(fs, 0, m_imageSize.height);
        cvEndWriteStruct(fs);
        //! Matriz intrínseca camara izquierda
        cvWrite(fs, C_INTRINSICS_LEFT, m_M1);  //! Matriz intrínseca de la cámara izquierda
        cvWrite(fs, C_DISTORTION_LEFT, m_D1);  //! Coeficientes de distorsión de la cámara izquierda
        cvWrite(fs, C_INTRINSICS_RIGHT, m_M2);  //! Matriz intrínseca de la cámara derecha
        cvWrite(fs, C_DISTORTION_RIGHT, m_D2);  //! Coeficientes de distorsión de la cámara derecha
        cvWrite(fs, C_ROTATION_MATRIX, m_R);  //! Matriz de rotación de la cámara derecha con respecto a la izquierda
        cvWrite(fs, C_TRASLATION_MATRIX, m_T);  //! Matriz de traslación de la cámara derecha con respecto a la izquierda
        cvWrite(fs, C_ESSENTIAL_MATRIX, m_E);  //! Matriz esencial
        cvWrite(fs, C_FUNDAMENTAL_MATRIX, m_F);  //! Matriz fundamental
        //! Mapas de interpolación para la rectificación
        cvWrite(fs, C_MX1, m_mx1);
        cvWrite(fs, C_MY1, m_my1);
        cvWrite(fs, C_MX2, m_mx2);
        cvWrite(fs, C_MY2, m_my2);

        cvReleaseFileStorage(&fs);
        return RESULT_OK;
    }
};

Persistence::Persistence()
{
    self = new pimp();
}

Persistence::~Persistence()
{
    delete self;
}


/*!
 * \brief Persistence::readCalibration For reading the calibration file, it could be a xml or yml file
 * \param type (C_FILE_TYPE_XML or C_FILE_TYPE_YML)
 * \return RESULT_OK or RESULT_FAIL
 */
int Persistence::loadCalibration(int type)
{
   return self->readCalibration(type);
}

int Persistence::saveCalibration(int type)
{
    return self->writeCalibration(type);
}

void Persistence::setCalibration(CvSize imageSize, CvMat *mx1, CvMat *my1, CvMat *mx2, CvMat *my2,
                                  CvMat *M1 = 0, CvMat *M2 = 0, CvMat *D1 = 0, CvMat *D2 = 0,
                                  CvMat *R = 0, CvMat *T = 0, CvMat *E = 0, CvMat *F = 0)
{
    self->m_imageSize = imageSize;
    self->m_mx1 = mx1;  self->m_my1 = my1;  self->m_mx2 = mx2;  self->m_my2 = my2;
    self->m_M1 = M1;    self->m_M2 = M2;    self->m_D1 = D1;    self->m_D2 = D2;
    self->m_R = R;      self->m_T = T;      self->m_E = E;      self->m_F = F;
}

void Persistence::getCalibration(Size &imageSize, Mat &mx1, Mat &my1, Mat &mx2, Mat &my2,
                                 Mat &M1, Mat &M2, Mat &D1, Mat &D2, Mat &R, Mat &T, Mat &E, Mat &F)
{
    imageSize = self->m_imageSize;
    mx1 = Mat(self->m_mx1);  my1 = Mat(self->m_my1);  mx2 = Mat(self->m_mx2);  my2 = Mat(self->m_my2);
    M1 = Mat(self->m_M1);    M2 = Mat(self->m_M2);    D1 = Mat(self->m_D1);    D2 = Mat(self->m_D2);
    R = Mat(self->m_R);      T = Mat(self->m_T);      E = Mat(self->m_E);      F = Mat(self->m_F);
}

/*!
 * \brief Persistence::getCalibration The params are in reference
 */
void Persistence::getCalibration(CvSize &imageSize, CvMat* &mx1, CvMat* &my1, CvMat* &mx2, CvMat* &my2,
                                 CvMat* &M1 , CvMat* &M2, CvMat* &D1, CvMat* &D2,
                                 CvMat* &R, CvMat* &T, CvMat* &E, CvMat* &F)
{
    imageSize = self->m_imageSize;
    mx1 = self->m_mx1;  my1 = self->m_my1;  mx2 = self->m_mx2;  my2 = self->m_my2;
    M1 = self->m_M1;    M2 = self->m_M2;    D1 = self->m_D1;    D2 = self->m_D2;
    R = self->m_R;      T = self->m_T;      E = self->m_E;      F = self->m_F;
}

void Persistence::getCalibration(CvSize &imageSize, CvMat *&mx1, CvMat *&my1, CvMat *&mx2, CvMat *&my2)
{
    imageSize = self->m_imageSize;
    mx1 = self->m_mx1;  my1 = self->m_my1;  mx2 = self->m_mx2;  my2 = self->m_my2;
}

Size Persistence::getImageSize()
{
    return self->n_imageSize;
}

Mat Persistence::getMx1()
{
    return self->n_mx1;
}

Mat Persistence::getMy1()
{
    return Mat(self->m_my1);
}

Mat Persistence::getMx2()
{
    return Mat(self->m_mx2);
}

Mat Persistence::getMy2()
{
    return Mat(self->m_my2);
}

Mat Persistence::getM1()
{
    return Mat(self->m_M1);
}

Mat Persistence::getM2()
{
    return Mat(self->m_M2);
}

Mat Persistence::getD1()
{
    return Mat(self->m_D1);
}

Mat Persistence::getD2()
{
    return Mat(self->m_D2);
}

Mat Persistence::getR()
{
    return Mat(self->m_R);
}

Mat Persistence::getT()
{
    return Mat(self->m_T);
}

Mat Persistence::getE()
{
    return Mat(self->m_E);
}

Mat Persistence::getF()
{
    return Mat(self->m_F);
}


