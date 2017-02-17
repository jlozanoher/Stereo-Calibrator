#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cv.h>
#include <cvaux.h>
#include <QFileDialog>

#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
    m_stereoCapture = new StereoCapture();
    m_stereoCalibrator = new StereoCalibrator();
    m_stereoProcess = new StereoProcess();

    m_timeToPicture = m_ui->timeImage->value();

    //! Conectar el timer con la función sl_timeout
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(sl_timeout()));
    //! Capturar los mensajes
    connect(this, SIGNAL(sg_message(QString)), this, SLOT(sl_captureMessage(QString)));
    connect(m_stereoCapture, SIGNAL(sg_message(QString)), this, SLOT(sl_captureMessage(QString)));
    connect(m_stereoCalibrator, SIGNAL(sg_message(QString)), this, SLOT(sl_captureMessage(QString)));
    //! Cada cambio en el spinbox se actualizará en la cuenta atrás
    connect(m_ui->timeImage, SIGNAL(valueChanged(int)), this, SLOT(sl_changeTimeToPicture(int)));
    //m_ui->checkBox->setChecked(true);
    on_buttonCamera_clicked();

    //!Depth map params
    connect(m_ui->dm_minDisparity, SIGNAL(valueChanged(int)), m_stereoProcess, SLOT(setMinDisparity(int)));
    connect(m_ui->dm_numDisp, SIGNAL(valueChanged(int)), m_stereoProcess, SLOT(setNumberOfDisparities(int)));
    connect(m_ui->dm_preFilterCap, SIGNAL(valueChanged(int)), m_stereoProcess, SLOT(setPreFilterCap(int)));
    connect(m_ui->dm_preFilterSize, SIGNAL(valueChanged(int)), m_stereoProcess, SLOT(setPreFilterSize(int)));
    connect(m_ui->dm_SADWindowSize, SIGNAL(valueChanged(int)), m_stereoProcess, SLOT(setSADWindowSize(int)));
    connect(m_ui->dm_spRange, SIGNAL(valueChanged(int)), m_stereoProcess, SLOT(setSpeckleRange(int)));
    connect(m_ui->dm_spWindowSize, SIGNAL(valueChanged(int)), m_stereoProcess, SLOT(setSpeckleWindowSize(int)));
    connect(m_ui->dm_textureThr, SIGNAL(valueChanged(int)), m_stereoProcess, SLOT(setTextureThreshold(int)));
    connect(m_ui->dm_uniqRatio, SIGNAL(valueChanged(int)), m_stereoProcess, SLOT(setUniquenessRatio(int)));
}

MainWindow::~MainWindow()
{
    delete m_stereoCapture;
    delete m_stereoCalibrator;
    delete m_ui;
}


//! Función que se ejecuta en cada timertick
void MainWindow::sl_timeout(){
    int aux = 0;
    if(m_stereoCapture->isReady()){

        if(m_stereoCapture->capture() == RESULT_OK){
            aux = SHOW_ONLY_CAMERA_IMAGES;            
            if(m_stereoCalibrator->getCalibratiorDone()) aux += SHOW_RECTIFICATION + SHOW_DEPTH_MAP;
            sl_displayImages(aux);
        }
        else{
            sl_captureMessage("Error con la captura.");
        }
    }
    //! Comienza la calibración
    if(m_stereoCalibrator->isCalibrationStarted()){
        //! Si es tiempo de tomar otro par
        if(m_timeToPicture <= 0){
            sl_changeTimeToPicture(m_ui->timeImage->value());
            sl_captureMessage(QString("Procesando el par %1 de %2 tomas.").arg(m_stereoCalibrator->getPairSuccess()+1).arg(m_stereoCalibrator->getPairCount()));
            //! En caso de que se encontraron todas las esquinas en la toma
            if(m_stereoCalibrator->calibrationAddPair(m_stereoCapture->getFrameGray(CAMERA_LEFT), m_stereoCapture->getFrameGray(CAMERA_RIGHT)) == RESULT_OK){
                sl_captureMessage("Ok.");
                //! Si ya se tomaron las muestras de imágenes necesarias se procede a la calibración
                if(m_stereoCalibrator->getPairSuccess() >= m_stereoCalibrator->getPairCount()){
                    if(m_stereoCalibrator->calibrationEnd() == RESULT_OK){
                        this->endedCalibration();
                    }
                    else {
                        sl_captureMessage("Error con la calibración");
                    }
                }
            }
            //! Falló la toma
            else{
                sl_captureMessage(QString::fromUtf8("Fallo, coloque el tablero en una zona visible para ambas cámaras."));
            }
        }
        //! Si no tomo otro par disminuye la cuenta atrás
        else{
            m_timeToPicture -= (double)m_timer.interval()/1000;
            m_ui->lcdNumber->display(ceil(m_timeToPicture));
        }
    }
}

void MainWindow::sl_captureMessage(QString message)
{
    m_ui->plainTextEdit->appendPlainText(message);
}


//! Slot que muestra las imágenes en los labels correspondientes
void MainWindow::sl_displayImages(int type)
{
    Mat rectifiedPair, depthMap;
    Mat imgL = m_stereoCapture->getFrame(CAMERA_LEFT);
    Mat imgR = m_stereoCapture->getFrame(CAMERA_RIGHT);
    Mat grayL = m_stereoCapture->getFrameGray(CAMERA_LEFT);
    Mat grayR = m_stereoCapture->getFrameGray(CAMERA_RIGHT);
    type /= 2;
    if(type % 2 == 1){
        //Rectification
        if(m_stereoProcess->process(grayL, grayR) == RESULT_OK){
            rectifiedPair = m_stereoProcess->getImageRectPair();
            depthMap = m_stereoProcess->getImageDepthNormalized();
            QPixmap pixRectified = QPixmap::fromImage( QImage(rectifiedPair.data, rectifiedPair.cols, rectifiedPair.rows, rectifiedPair.step, QImage::Format_RGB888));
            m_ui->displayRectification->setPixmap(pixRectified.scaled(m_ui->displayRectification->size(), Qt::KeepAspectRatio));
        }
        else {
            sl_captureMessage(QString::fromUtf8("Imposible realizar la rectificación"));
        }
    }
    type /= 2;
    if(type % 2 == 1){
        //Depth
        cvtColor(depthMap, depthMap, CV_GRAY2BGR);
        QImage depthImage(depthMap.data, depthMap.cols, depthMap.rows, depthMap.step, QImage::Format_RGB888);
        QPixmap pixDepthMap = QPixmap::fromImage(depthImage);
        m_ui->displayDepthMap->setPixmap(pixDepthMap.scaled(m_ui->displayDepthMap->size(), Qt::KeepAspectRatio));
    }
    //Show input images

    const QPixmap pixLeft = QPixmap::fromImage(QImage(imgL.data, imgL.cols, imgL.rows, imgL.step, QImage::Format_RGB888));
    const QPixmap pixRight = QPixmap::fromImage(QImage(imgR.data, imgR.cols, imgR.rows, imgR.step, QImage::Format_RGB888));
    const QSize size = m_ui->displayLeft->size();
    m_ui->displayLeft->setPixmap(pixLeft.scaled(size,Qt::KeepAspectRatio));
    m_ui->displayRight->setPixmap(pixRight.scaled(size,Qt::KeepAspectRatio));    
}

//! Cambia el tiempo con que se toman las imágenes de calibración
void MainWindow::sl_changeTimeToPicture(int time)
{
    m_timeToPicture = time;
    m_ui->lcdNumber->display(time);
}

//! Acción de iniciar cámaras
void MainWindow::on_buttonCamera_clicked()
{
    int cameraRight = CAMERA_RIGHT + 1;
    if(m_ui->checkBox->isChecked()) cameraRight = 0;
    if(m_stereoCapture->setup(CAMERA_LEFT + 1, cameraRight) == RESULT_OK){
        sl_captureMessage(QString::fromUtf8("Cámaras conectadas"));
        m_ui->buttonCalibration->setEnabled(true);        
        //! Ojo con el intervalo del timeout !!
        m_timer.start(100);
    }
    else{
        sl_captureMessage("Error con las cámaras.");
    }
}

void MainWindow::on_buttonCalibration_clicked()
{
    m_stereoCalibrator->calibrationStart(m_stereoCapture->getImageSize(), m_ui->pairCount->value(), m_ui->cornersX->value(), m_ui->cornersY->value(), m_ui->squareSize->value());
    sl_captureMessage(QString::fromUtf8("Comenzando la calibración, colóque los tableros para que estén visibles en ambas cámaras."));
    sl_captureMessage(QString::fromUtf8("Se tomarán imágenes cada %1 segundos.").arg((int)m_timeToPicture));
    m_ui->buttonCalibration->setEnabled(false);
    m_ui->squareSize->setEnabled(false);
    m_ui->cornersX->setEnabled(false);
    m_ui->cornersY->setEnabled(false);
    m_ui->pairCount->setEnabled(false);
    m_ui->squareSize->setEnabled(false);
}

/*!
 * \brief For saving calibration to xml or yml file
 */
void MainWindow::on_buttonSave_clicked()
{
    if(m_stereoCalibrator->getCalibratiorDone()){
        int type = C_FILE_TYPE_YML;
        QString typeStr = "YML files (*.yml)";
        if(m_ui->radioButtonXML->isChecked()){
            typeStr = "XML files (*.xml)";
            type = C_FILE_TYPE_XML;
        }

        QString path = QFileDialog::getSaveFileName(this,
                                                     "Selecciones la carpeta de salva",
                                                     "calibration", typeStr);

        if(m_stereoCalibrator->saveCalibration(path, type) == RESULT_OK) sl_captureMessage(QString::fromUtf8("Calibración salvada con éxito"));
        else sl_captureMessage(QString::fromUtf8("Error salvando la calibración"));
    }
    else{
        sl_captureMessage(QString::fromUtf8("La calibración no ha terminado"));
    }
}


/*!
 * \brief For loading calibration from xml or yml file
 */
void MainWindow::on_buttonLoad_clicked()
{
    if(m_stereoCapture->isReady()){       
        int type = C_FILE_TYPE_YML;
        QString typeStr = "YML files (*.yml)";
        if(m_ui->radioButtonXML->isChecked()){
            typeStr = "XML files (*.xml)";
            type = C_FILE_TYPE_XML;
        }

        QString path = QFileDialog::getOpenFileName(this,
                                                     "Selecciones el archivo de calibración",
                                                     "calibration", typeStr);

        if(m_stereoCalibrator->loadCalibration(path, type) == RESULT_OK){
            //! Putting the calibration into the stereoProcess
            this->setCalibration();
            m_stereoCalibrator->setCalibrationDone(true);
            sl_captureMessage(QString::fromUtf8("Calibración cargada con éxito"));
        }
        else{
            sl_captureMessage(QString::fromUtf8("Error cargando la calibración"));
        }
    }
}

void MainWindow::setCalibration()
{
    //! Putting the calibration into the stereoProcess
    m_stereoProcess->setCalibration(
                m_stereoCalibrator->getImageSize(),
                m_stereoCalibrator->getMx1(),
                m_stereoCalibrator->getMy1(),
                m_stereoCalibrator->getMx2(),
                m_stereoCalibrator->getMy2()
                );
}

void MainWindow::endedCalibration()
{
    sl_captureMessage(QString::fromUtf8("Calibración con éxito."));
    this->setCalibration();
    m_ui->buttonCalibration->setEnabled(true);
    m_ui->squareSize->setEnabled(true);
    m_ui->cornersX->setEnabled(true);
    m_ui->cornersY->setEnabled(true);
    m_ui->pairCount->setEnabled(true);
    m_ui->squareSize->setEnabled(true);
}
