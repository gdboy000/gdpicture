//
// Created by gao on 24-11-11.
//

#ifndef CANVASLABEL_H
#define CANVASLABEL_H

#include <QImage>

#include "clayer.h"
#include "coverlayer.h"

/**
 *  画布层
 *  功能：显示图像，支持右键拖拉移动，滚轮+ctrl缩放
 */
class CanvasLayer: public CLayer {
    Q_OBJECT
public:
    explicit  CanvasLayer(QWidget *parent = nullptr);
    ~CanvasLayer() override;
    bool ShowImage(const QString &);//reset file path
    void setImage(const QImage&);//获取图像
    void showImage();
    void showImage(const QImage&);//获取并展示图像
    void Zoom(int);
    void SizeChanged(QSize);
    void setMaskLayer(CoverLayer* label){_mask = label;}
    void setHigherSize(const QSize&);//获取上层大小
    // void setPosition(const QPoint &) override;
    // void setSize(const QSize&) override;
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void hideEvent(QHideEvent *event) override{ if(nullptr!=_mask)_mask->hide();}
    void moveEvent(QMoveEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
private:
    CoverLayer* _mask = nullptr;
    QSize   _parentSize = {0,0};//parent widget size
    QSizeF _currentParentSize;
    QImage _image;
    QImage _prototypeImage;//原型图像
    QSizeF _prototypeImageSize;//原型图像分辨率
    QSizeF _vectorDirection;
    double _prototypeImageAspectRatio;//原型图像纵横比
    double _imageWidth;
    double _imageHeight;
    double tmp1;
    double tmp2;
    QSizeF _currentImageSize;
    bool _vertical = false;
    bool _rigthMouseClicked = false;
    QString _filePath;
    const double _pulleySpan = 0.01;
    //当前缩放比率
    double _currentZoomRatio = 0.00;//Current image zoom ratio
    /**
     * 当图像宽度获高度到达上层边缘时
     * 记录当前缩放比率
     */
    double _limitMaxZoomRatio = 0.00;//Maximum zoom ratio for label
    //Minimum zoom ratio for label
    double _limitMinZoomRatio = 0.00;//画面最小占比
    // //Default zoom ratio
    double _defaultPercentage = 0.80;//画面默认占比
    //aspect ratio
    double _ratio = 0.00;//图像当前纵横比
    double _x,_y;//Current label location
    QPoint _startPoint,_endPoint,_currentPoint;
    int _offsetX = 0;
    int _offsetY = 0;
    int _currentOffsetX = 0;
    int _currentOffsetY = 0;
    QSize _labelSize;//Current label size
    bool _reLayout(double zoomLevel = 0.00);
    bool _resizeLabel(double);//resize label rely on ratio arg
    bool _resizeLabel(int,int);//resize label with size
    bool _resizeLabel(QSize size){return _resizeLabel(size.width(),size.height()); }
    void _move();


public slots:

signals:
    void sendMessage(QString);
    void sizeChanges(const QSize&);
    void positionChanges(const QPoint&);





};



#endif //CANVASLABEL_H
