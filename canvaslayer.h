//
// Created by gao on 24-11-11.
//

#ifndef CANVASLABEL_H
#define CANVASLABEL_H

#include <QImage>

#include "clayer.h"

/**
 *  画布层
 *  功能：显示图像，支持右键拖拉移动，滚轮+ctrl缩放
 */
class CanvasLayer: public CLayer {
    Q_OBJECT
public:
    explicit  CanvasLayer(QWidget *parent = nullptr);
    ~CanvasLayer() override;
    void setImage(const QImage&);//设置图像
    void showImage();//展示图像
    void showImage(const QImage&);//设置并展示图像
    void setHigherSize(const QSize&);//设置上层大小
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void moveEvent(QMoveEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
private:
    QSize  _parentSize = {0,0};//parent widget size
    QSizeF _currentParentSize;
    QImage _prototypeImage;//原型图像
    QSizeF _prototypeImageSize;//原型图像分辨率
    double _prototypeImageAspectRatio;//原型图像纵横比
    QSizeF _currentImageSize;
    bool _rigthMouseClicked = false;
    const double _pulleySpan = 0.01;

    double _currentZoomRatio = 0.00;//当前缩放比率
    double _defaultPercentage = 0.80;//画面默认占比
    double _x,_y;//Current label location
    QPoint _startPoint,_endPoint,_currentPoint;
    int _offsetX = 0;
    int _offsetY = 0;
    bool _reLayout(double zoomLevel = 0.00);
    bool _resizeLabel(int,int);//resize label with size
    bool _resizeLabel(QSize size){return _resizeLabel(size.width(),size.height()); }
    void _move();
signals:
    void sendMessage(QString);
    void sizeChanges(const QSize&);//当前图层大小改变信号
    void positionChanges(const QPoint&);//当前图层位置改变信号
    void zoomRatioChanges(double);//当前图像缩放比率改变信号
};



#endif //CANVASLABEL_H
