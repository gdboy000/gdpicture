//
// Created by gao on 24-11-12.
//

#ifndef COVERLABEL_H
#define COVERLABEL_H

#include <QLabel>


class CoverLabel :public QLabel{
    Q_OBJECT
public:
    explicit CoverLabel(QWidget *parent = nullptr);
    ~CoverLabel();

    void setPartnerGeometry(int,int,int,int);
    void setPartnerGeometry(const QPoint& point,const QSize& size){setPartnerGeometry(point.x(),point.y(),size.width(),size.height());}
    void setPartnerGeometry(int x,int y,const QSize& size){setPartnerGeometry(x,y,size.width(),size.height());}
    void setPartnerGeometry(const QPoint& point,int width,int height){setPartnerGeometry(point.x(),point.y(),width,height);}

    void setDefaultGeometry();

    void SizeChanged(QSize);
    void Move(int,int);
    void Zoom(double,double);
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

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
