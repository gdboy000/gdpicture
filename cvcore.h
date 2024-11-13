//
// Created by gao on 24-11-11.
//

#ifndef CVCORE_H
#define CVCORE_H
#include <QImage>
#include <opencv2/core.hpp>

using namespace cv;

bool QImage2Mat(const QImage &image, Mat &mat);
bool Mat2QImage(const Mat &mat, QImage &image);

bool pixelZoomIn(const QImage& imgae, QImage& zoomImage,int ratio);
bool pixelZoomOut(const QImage& imgae, QImage& zoomImage,int ratio);
#endif //CVCORE_H
