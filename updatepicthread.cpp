#if _MSC_VER >= 1900
#pragma execution_character_set("utf-8")
#endif
#include "updatepicthread.h"
#include <QPainter>
#include <QRect>
#include <QBrush>
#include <QFont>
#include <QPainter>
#include "ui_mainwindow.h"

static Mat M1, M2, D1, D2, R, T;//图像矫正参数
static Mat disp8;
double decPosArr[6] = {0, 0, 0, 0, 0, 0};

UpdatePicThread::UpdatePicThread(Ui::MainWindow *ui)
{
   gui=ui;
}

UpdatePicThread::~UpdatePicThread()
{

}

/*图片刷新*/

void UpdatePicThread::run()
{
    //打开相机
    cap.open(0);
    cap.set(CAP_PROP_FRAME_WIDTH,1280);
    cap.set(CAP_PROP_FRAME_HEIGHT,480);
    //while(!cap.isOpened());
    if(cap.isOpened())
    {
       qDebug()<<tr("开启相机");
    }
    //else
   // {
       //QMessageBox::warning(this,"Warning","Opening camera failed",QMessageBox::Yes);
       // qDebug()<<tr("未开启相机");
   // }

    while(cap.isOpened())
    {
        //获取图像
        cap>>frame;
        frameLeft=frame(Rect(0,0,640,480));
        frameRight=frame(Rect(640,0,640,480));

        //转换为灰度图像
        cvtColor(frameLeft,grayLeft,COLOR_BGR2GRAY);
        cvtColor(frameRight,grayRight,COLOR_BGR2GRAY);
        imwrite("left.jpg",grayRight);
        //imshow("  ",grayLeft);

        //图像校正
        rectify();

        matchSGBM();
        //获取二值图并剪切
        Cut();


    }
}


void UpdatePicThread::updateImage()
{
    //获取图像
    cap>>frame;
    frameLeft=frame(Rect(0,0,640,480));
    frameRight=frame(Rect(640,0,640,480));

    //转换为灰度图像
    cvtColor(frameLeft,grayLeft,COLOR_BGR2GRAY);
    cvtColor(frameRight,grayRight,COLOR_BGR2GRAY);

    //图像校正
    rectify();

    //转换为RGB图像
    cvtColor(rectLeft,RGBLeft,COLOR_BGR2RGB);
    cvtColor(rectRight,RGBRight,COLOR_BGR2RGB);

    //获取二值图并剪切
    Cut();

    //三维重构
    if((gui->tabWidget->currentIndex() == 0)|(gui->tabWidget->currentIndex() == 4))
        matchSGBM();


}

void UpdatePicThread::readParameters()
{
   //读取左相机内参数
    M1 = (Mat_<double>(3, 3) << 411.9248, -0.3882, 416.2442, 0, 412.0844, 271.5919, 0, 0, 1);
    //读取右相机内参数
    M2 = (Mat_<double>(3, 3) << 409.2776, -0.9822, 332.6745, 0, 409.2971, 230.8318, 0, 0, 1);
    //读取左相机畸变参数
    D1 = (Mat_<double>(5, 1) << -0.3966, 0.2176, 0, -0.0022, 0);
    //读取右相机畸变参数
    D2 = (Mat_<double>(5, 1) << -0.3861, 0.2013, -0.0012, 0.0030, 0);
    //读取旋转矩阵
    R = (Mat_<double>(3, 3) << 1, 0, -0.0016, 0, 1, 0.0151, 0.0016, -0.0151, 1);
    //读取平移矩阵
    T = (Mat_<double>(3, 1) << 62.4653, 0.4548, 0.6288);
}

void UpdatePicThread::rectify()
{
    //立体矫正
    //cvtColor(frameLeft,grayLeft,COLOR_BGR2GRAY);
    //cvtColor(frameRight,grayRight,COLOR_BGR2GRAY);

    Rect roi1, roi2;
    Mat Q, R1, R2, P1, P2;
    readParameters();
    stereoRectify(M1, D1, M2, D2, Size(grayLeft.cols, grayLeft.rows), R, T, R1, R2, P1, P2, Q,
                  CALIB_ZERO_DISPARITY, -1.0, Size(grayLeft.cols, grayLeft.rows), &roi1, &roi2);

    //矫正映射
    Mat map11, map12, map21, map22;
    initUndistortRectifyMap(M1, D1, R1, P1, Size(grayLeft.cols, grayLeft.rows), CV_16SC2, map11, map12);
    initUndistortRectifyMap(M2, D2, R2, P2, Size(grayLeft.cols, grayLeft.rows), CV_16SC2, map21, map22);

    remap(grayLeft, rectLeft, map11, map12, INTER_LINEAR);
    remap(grayRight, rectRight, map21, map22, INTER_LINEAR);
    //flip(rectLeft,rectLeft,-1);
    cvtColor(rectLeft,RGBLeft,COLOR_BGR2RGB);
    cvtColor(rectRight,RGBRight,COLOR_BGR2RGB);

}

void UpdatePicThread::Cut()
{
    //QString str=ui->thresholdValueBox->text();
    //int value=str.toInt();
    Mat blurImg;
    medianBlur(rectLeft, blurImg, 3);//中值滤波
    Ptr<CLAHE> clahe=createCLAHE(2, Size(8, 8));
    clahe->apply(blurImg,blurImg);
    int value=150;
    Mat imageThreshold,imageNew;
    //threshold(blurImg, imageThreshold, value, 255, THRESH_BINARY);
    adaptiveThreshold(blurImg, imageThreshold, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 41, 1);
    /*
    imageNew = Mat::zeros(rectLeft.size(), rectLeft.type());
    for (int i = 0; i < rectLeft.rows; i++)
    {
        for (int j = 0; j < rectLeft.cols; j++)
        {
            int src1Ele = imageThreshold.at<uchar>(i, j);
            int src2Ele = rectLeft.at<uchar>(i, j);
            if (src1Ele < 50)
                imageNew.at<uchar>(i, j) = uchar(src2Ele);
            else
                imageNew.at<uchar>(i, j) = 255;
        }
    }
    */
    cvtColor(imageThreshold,imageCut,COLOR_GRAY2RGB);
    //flip(imageCut,imageCut,-1);
    //imshow("Image Cut", imageNew);
}

void UpdatePicThread::matchSGBM()
{
    Mat SGBMLeft, SGBMRight;
    SGBMLeft.create(RGBLeft.rows, RGBLeft.cols, CV_8UC1);
    SGBMRight.create(RGBLeft.rows, RGBLeft.cols, CV_8UC1);
    rectLeft.copyTo(SGBMLeft);
    rectRight.copyTo(SGBMRight);

    Ptr<cv::StereoSGBM> sgbm = StereoSGBM::create(16, 9);
    int SADWindowSize = 7;//
    int numberOfDisparities = 32;//
    sgbm->setPreFilterCap(63);
    int sgbmWinSize = SADWindowSize > 0 ? SADWindowSize : 3;
    sgbm->setBlockSize(sgbmWinSize);
    int cn = SGBMLeft.channels();
    sgbm->setP1(8 * cn*sgbmWinSize*sgbmWinSize);
    sgbm->setP2(32 * cn*sgbmWinSize*sgbmWinSize);
    sgbm->setMinDisparity(0);
    sgbm->setNumDisparities(numberOfDisparities);
    sgbm->setUniquenessRatio(10);//
    sgbm->setSpeckleWindowSize(100);
    sgbm->setSpeckleRange(32);
    sgbm->setDisp12MaxDiff(1);
    int64 t = getTickCount();

    sgbm->compute(SGBMLeft, SGBMRight, disp);
    GaussianBlur(disp,disp,Size(3,3),0,0,BORDER_DEFAULT);
    //insertDepth32f(disp);
    t = getTickCount() - t;
    //printf("Time elapsed: %fms\n", t * 1000 / getTickFrequency());
    disp.convertTo(disp8, CV_8U, 255 / (numberOfDisparities*16.));
    //imshow("result", disp8);
    cvtColor(disp8,dispRGB,COLOR_GRAY2RGB);

}

void UpdatePicThread::paintEvent(QPaintEvent *e)
{
    QImage imageLeft(RGBLeft.data,RGBLeft.cols,RGBLeft.rows,QImage::Format_RGB888);
    gui->picLeft2->setPixmap(QPixmap::fromImage(imageLeft));
    gui->picLeft2->resize(imageLeft.size());
    gui->picLeft2->show();
    imageLeft=imageLeft.scaled(320,240);
    gui->picLeft->setPixmap(QPixmap::fromImage(imageLeft));
    gui->picLeft->resize(imageLeft.size());
    gui->picLeft->show();

    QImage imageRight(RGBRight.data,RGBRight.cols,RGBRight.rows,QImage::Format_RGB888);
    gui->picRight2->setPixmap(QPixmap::fromImage(imageRight));
    gui->picRight2->resize(imageRight.size());
    gui->picRight2->show();
    imageRight=imageRight.scaled(320,240);
    gui->picRight->setPixmap(QPixmap::fromImage(imageRight));
    gui->picRight->resize(imageRight.size());
    gui->picRight->show();

    QImage imageCutQ(imageCut.data,imageCut.cols,imageCut.rows,QImage::Format_RGB888);
    gui->picCut2->setPixmap(QPixmap::fromImage(imageCutQ));
    gui->picCut2->resize(imageCutQ.size());
    gui->picCut2->show();
    imageCutQ=imageCutQ.scaled(320,240);
    gui->picCut->setPixmap(QPixmap::fromImage(imageCutQ));
    gui->picCut->resize(imageCutQ.size());
    gui->picCut->show();


    QImage imageDisp(dispRGB.data,dispRGB.cols,dispRGB.rows,QImage::Format_RGB888);
    gui->picPcl2->setPixmap(QPixmap::fromImage(imageDisp));
    gui->picPcl2->resize(imageDisp.size());
    gui->picPcl2->show();
    imageDisp=imageDisp.scaled(320,240);
    gui->picPcl->setPixmap(QPixmap::fromImage(imageDisp));
    gui->picPcl->resize(imageDisp.size());
    gui->picPcl->show();

}








