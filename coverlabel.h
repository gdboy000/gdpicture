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
    int _width,_height;
    int _currentWidth,_currentHeight;
    int _partnerWidth,_partnerHeight;
    int _partnerX,_partnerY;
};



#endif //COVERLABEL_H
