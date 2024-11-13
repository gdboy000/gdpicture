//
// Created by gao on 24-11-11.
//
#include "cvcore.h"

bool pixelZoomIn(const QImage& imgae, QImage& zoomImage,int ratio)
{
    Mat src;
    if(QImage2Mat(imgae,src)) {
        Mat dst = Mat(imgae.height()*ratio,imgae.width()*ratio,CV_8UC4);
        for(int x=0;x<src.cols;x++)
            for(int y=0;y<src.rows;y++) {
                auto color = src.at<Vec4b>(y,x);
                for(int x_add=0;x_add<ratio;x_add++)
                    for(int y_add=0;y_add<ratio;y_add++)
                        dst.at<Vec4b>(y*ratio+y_add,x*ratio+x_add) = color;
            }
        return Mat2QImage(dst,zoomImage);
    }
    return false;

}
bool pixelZoomOut(const QImage& imgae, QImage& zoomImage,int ratio)
{
    int width = imgae.width()/ratio;
    int height = imgae.height()/ratio;
    zoomImage = imgae.scaled(width,height);
}