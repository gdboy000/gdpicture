//
// Created by gao on 24-11-12.
//

#ifndef COVERLABEL_H
#define COVERLABEL_H

#include "clayer.h"

//遮盖层
class CoverLayer :public CLayer{
    Q_OBJECT
public:
    explicit CoverLayer(QWidget *parent = nullptr);
    ~CoverLayer();
    void setRestrictedLayer(const CLayer*);
    void setRestrictedRange(int,int,int,int);
    void setRestrictedRange(const QPoint& point,const QSize& size){setRestrictedRange(point.x(),point.y(),size.width(),size.height());}
    void setRestrictedRange(int x,int y,const QSize& size){setRestrictedRange(x,y,size.width(),size.height());}
    void setRestrictedRange(const QPoint& point,int width,int height){setRestrictedRange(point.x(),point.y(),width,height);}
    void setDefaultGeometry();
    void effectBySizeOfLayer(const QSize&);//受其它层大小影响
    void effectByPosOfLayer(const QPoint&);//受其它层位置影响
    void SizeChanged(QSize);
    void Move(int,int);
    void Zoom(double,double);
    void setPosition(const QPoint &) override;
    void setSize(const QSize &) override;
    void setOffset(const QPoint &,int) override;
protected:
    void paintEvent(QPaintEvent *event) override;
    // void mousePressEvent(QMouseEvent *event) override;
    // void mouseMoveEvent(QMouseEvent *event) override;
    // void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool _onLeftMouseBtn = false;
    int _direction;
    QPoint _startPoint,_endPoint,_currentPoint;
    void _move();
    void _resize(int);
    int _x,_y;
    int _currentX,_currentY;
    double _width,_height;
    int _partnerX,_partnerY;
    double _currentWidth,_currentHeight;
    double _partnerWidth,_partnerHeight;
};



#endif //COVERLABEL_H
