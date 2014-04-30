#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QtCore>

#include "opencv2/core/core_c.h"
#include "opencv2/core/core.hpp"
#include "opencv2/flann/miniflann.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/contrib/contrib.hpp"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    capWebcam.open(0);      // for ip cam change "0" to  http://user1:secret23@192.168.0.14/axis-cgi/mjpg/video.cgi?resolution=640x480&req_fps=30&.mjpg
    if(capWebcam.isOpened() == false) {
        ui->txtXYRadius ->appendPlainText("erroor  no cam");
        return;
    }
    tmrTimer = new QTimer(this);
    connect(tmrTimer, SIGNAL (timeout()), this, SLOT(processFrameAndUpdateGUI()));
         ui->txtXYRadius ->appendPlainText("good cam");
    tmrTimer->start(20);
}

MainWindow::~MainWindow()
{
    delete ui;
}






void MainWindow::processFrameAndUpdateGUI() {
    capWebcam.read(matOriginal);
    if(matOriginal.empty() == true ) return;
    cv::inRange(matOriginal,				// funcion input
                cv::Scalar(0,0,175),		// min filtering value (if greater than or equal to this) (in BGR format)
                cv::Scalar(100,100,256),	// max filtering value (and if less than this) (in BGR format)
                matProcessed);				// function output

                                        // smooth the processed image, this will make it easier for the next function to identify circles
    cv::GaussianBlur(matProcessed,			// function input
                     matProcessed,			// function output
                     cv::Size(9,9),			// smoothing window width and height in pixels
                     1.5);					// sigma value, determines how much the image will be blurred

                                                // fill circles vector with all circles in processed image
    cv::HoughCircles(matProcessed,				// input image
                     vecCircles,				// function output (must be a standard template library vector
                     CV_HOUGH_GRADIENT,			// two-pass algorithm for detecting circles, this is the only choice available
                     2,							// size of image / this value = "accumulator resolution", i.e. accum res = size of image / 2
                     matProcessed.rows / 4,		// min distance in pixels between the centers of the detected circles
                     100,						// high threshold of Canny edge detector (called by cvHoughCircles)
                     50,						// low threshold of Canny edge detector (set at 1/2 previous value)
                     10,						// min circle radius (any circles with smaller radius will not be returned)
                     400);						// max circle radius (any circles with larger radius will not be returned)

    for(itrCircles = vecCircles.begin(); itrCircles != vecCircles.end(); itrCircles++) {
        ui->txtXYRadius->appendPlainText(QString("ball position x") + QString::number((*itrCircles)[0]).rightJustified(4, ' ') +
                                         QString(", y =") + QString::number((*itrCircles)[1]).rightJustified(4, ' ') +
                                         QString(", radius =") + QString::number((*itrCircles)[2], 'f', 3).rightJustified(7, ' '));


                cv::circle(matOriginal,													// draw on original image
                           cv::Point((int)(*itrCircles)[0], (int)(*itrCircles)[1]),		// center point of circle
                           3,															// radius of circle in pixels
                           cv::Scalar(0,255,0),											// draw pure green (remember, its BGR, not RGB)
                           CV_FILLED);													// thickness, fill in the circle

                                                    // draw red circle around the detected object
                cv::circle(matOriginal,													// draw on original image
                           cv::Point((int)(*itrCircles)[0], (int)(*itrCircles)[1]),		// center point of circle
                           (int)(*itrCircles)[2],										// radius of circle in pixels
                           cv::Scalar(0,0,255),											// draw pure red (remember, its BGR, not RGB)
                           3);															// thickness of circle in pixels

                }
    cv::cvtColor(matOriginal, matOriginal, CV_BGR2RGB);
    QImage qimgOriginal((uchar*)matOriginal.data, matOriginal.cols, matOriginal.rows, matOriginal.step, QImage::Format_RGB888);
    QImage qimgProcessed((uchar*)matProcessed.data, matProcessed.cols, matProcessed.rows, matProcessed.step, QImage::Format_Indexed8);
    ui->lblOriginal->setPixmap(QPixmap::fromImage(qimgOriginal));
    ui->lblProcessed->setPixmap(QPixmap::fromImage(qimgProcessed));

 }

void MainWindow::on_btnPauseOrResume_clicked()
{
    if(tmrTimer->isActive() == true){
        tmrTimer->stop();
        ui->btnPauseOrResume->setText("resume");
    }
    else {
        tmrTimer->start(20);
        ui->btnPauseOrResume->setText("pause");
    }

}





