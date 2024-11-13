//
// Created by gao on 24-11-11.
//
#include "cvcore.h"

bool Mat2QImage(const Mat &mat, QImage &image)
{
    if(mat.empty()) return false;
    if (mat.type() == CV_8UC1)
    {
        image = QImage(mat.cols, mat.rows, QImage::Format_Indexed8);
        image.setColorCount(256);
        for (int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        uchar* pSrc = mat.data;
        for (int row = 0; row < mat.rows; row++)
        {
            uchar* pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return true;
    }
    if (mat.type() == CV_8UC3)
    {
        const uchar* pSrc = mat.data;
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        image = image.rgbSwapped();
        return true;
    }
    if (mat.type() == CV_8UC4)
    {
        const uchar* pSrc = (const uchar*)mat.data;
        image = QImage(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return true;
    }
    return false;
}

bool QImage2Mat(const QImage &image, Mat &mat)
{
    if(image.isNull()) return false;
    try {
        QImage image = image.convertToFormat(QImage::Format_ARGB32);
        mat = cv::Mat(image.height(),image.width(),CV_8UC4,image.bits(),image.bytesPerLine());
        return true;
    }catch(cv::Exception e) {
        return false;
    }
}
