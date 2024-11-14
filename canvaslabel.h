//
// Created by gao on 24-11-11.
//

#ifndef CANVASLABEL_H
#define CANVASLABEL_H

#include <QLabel>
#include <QImage>

class CoverLabel;

class CanvasLabel: public QLabel {
    Q_OBJECT
public:
    explicit  CanvasLabel(QWidget *parent = nullptr);
    ~CanvasLabel() override;
    bool ShowImage(const QString &);//reset file path
    void Zoom(int);
    void SizeChanged(QSize);
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
private:
    CoverLabel* _mask;
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
    bool _layoyt(double percentage = 0.00);//set label layout
    bool _resizeLabel(double);//resize label rely on ratio arg
    bool _resizeLabel(int,int);//resize label with size
    bool _resizeLabel(QSize size){return _resizeLabel(size.width(),size.height()); }
    void _move();
    void _startMove();
    void _stopMove();

public slots:

signals:
    void sendMessage(QString);





};



#endif //CANVASLABEL_H
