//
// Created by gao on 24-11-12.
//
#include <QPaintEvent>
#include <QPainter>

#include "coverlabel.h"

CoverLabel::CoverLabel(QWidget *parent):
    QLabel(parent){
    //设置鼠标检测
    setMouseTracking(true);
    _x = 0;
    _y = 0;
    _currentX = 0;
    _currentY = 0;
    _currentWidth = 0;
    _currentHeight = 0;
    _partnerWidth = 0;
    _partnerHeight = 0;
}

CoverLabel::~CoverLabel() {

}

void CoverLabel::setPartnerGeometry(int x, int y, int width, int height) {
    if(0 != _partnerWidth && 0 != _partnerHeight) {
        //等比例操作 原图横宽/原伙伴横宽 == 现在横宽/现在伙伴横宽
        double ratio = width/_partnerWidth;
        _currentWidth = qRound(_currentWidth*ratio);
        _currentHeight = qRound(_currentHeight*ratio);
        _currentX = qRound(_currentX*ratio);
        _currentY = qRound(_currentY*ratio);
    }
    //因为_currentX和_currentY是相对于partner窗口的，因此x,y改变不影响
    _partnerX = x;
    _partnerY = y;
    _partnerWidth = width;
    _partnerHeight = height;
    this->resize(_currentWidth,_currentHeight);
    this->move(_currentX+_partnerX,_currentY+_partnerY);
}

void CoverLabel::setDefaultGeometry() {
    _currentX = 0;
    _currentY = 0;
    _currentWidth = _partnerWidth;
    _currentHeight = _partnerHeight;
    this->resize(_currentWidth,_currentHeight);
    this->move(_currentX+_partnerX,_currentY+_partnerY);
}


void CoverLabel::SizeChanged(QSize size) {
    //区分是图像主动进行的size改变，还是窗口
    _currentX = 0;
    _currentY = 0;
    _partnerWidth = size.width();
    _partnerHeight = size.height();
    _currentWidth = size.width();
    _currentHeight = size.height();

}

void CoverLabel::Move(int x,int y) {
    _partnerX = x;
    _partnerY = y;
    this->move(_currentX+_partnerX,_currentY+_partnerY);
}

void CoverLabel::Zoom(double srcP, double dstP) {

}


void CoverLabel::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(5);
    pen.setBrush(QColor(250,150,150));
    painter.setPen(pen);
    int width = this->width();
    int height = this->height();
    QLine line1(QPoint(0,0),QPoint(0,30));
    QLine line2(QPoint(0,0),QPoint(30,0));
    QLine line3(QPoint(width,0),QPoint(width,30));
    QLine line4(QPoint(width,0),QPoint(width-30,0));
    QLine line5(QPoint(0,height),QPoint(30,height));
    QLine line6(QPoint(0,height),QPoint(0,height-30));
    QLine line7(QPoint(width,height),QPoint(width-30,height));
    QLine line8(QPoint(width,height),QPoint(width,height-30));

    QLine line9(QPoint(width*1/3,0),QPoint(width*1/3,height));
    QLine line10(QPoint(width*2/3,0),QPoint(width*2/3,height));
    QLine line11(QPoint(0,height*1/3),QPoint(width,height*1/3));
    QLine line12(QPoint(0,height*2/3),QPoint(width,height*2/3));
    painter.drawLine(line1);
    painter.drawLine(line2);
    painter.drawLine(line3);
    painter.drawLine(line4);
    painter.drawLine(line5);
    painter.drawLine(line6);
    painter.drawLine(line7);
    painter.drawLine(line8);
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(1);
    pen.setBrush(QColor(150,150,150));
    painter.setPen(pen);
    painter.drawLine(line9);
    painter.drawLine(line10);
    painter.drawLine(line11);
    painter.drawLine(line12);

    _width = this->width();
    _height = this->height();
    QLabel::paintEvent(event);




}

void CoverLabel::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        _onLeftMouseBtn = true;
        _startPoint = QCursor::pos(); // 记录点击位置
        event->accept();
    } else {
        QWidget::mousePressEvent(event);
    }
}

void CoverLabel::mouseMoveEvent(QMouseEvent *event) {
    if(!_onLeftMouseBtn) {
        int width = this->width();
        int height = this->height();
        int x = event->pos().x();
        int y = event->pos().y();
        //该部分是否可以写进css中
        if(x>=0 && x<=10 && y>=0 && y<=10) {
            this->setCursor(Qt::SizeFDiagCursor);
            _direction = 7;
        }
        else if(x>=width-10 && x<=width && y>=height-10 && y<=height) {
            this->setCursor(Qt::SizeFDiagCursor);
            _direction = 3;
        }
        else if(x>=width-10 && x<=width && y>=0 && y<=10) {
            this->setCursor(Qt::SizeBDiagCursor);
            _direction = 9;
        }
        else if(x>=0 && x<=10 && y>=height-10 && y<=height) {
            this->setCursor(Qt::SizeBDiagCursor);
            _direction = 1;
        }
        else if(x>10 && x<width-10 && y<10 && y>0) {
            this->setCursor(Qt::SizeVerCursor);
            _direction = 8;
        }
        else if(x>10 && x<width-10 && y>height-10 && y<height) {
            this->setCursor(Qt::SizeVerCursor);
            _direction = 2;
        }
        else if(y>10 && y<width-10 && x>0 && x<10) {
            this->setCursor(Qt::SizeHorCursor);
            _direction = 4;
        }
        else if(y>10 && y<width-10 && x<width && x>width-10) {
            this->setCursor(Qt::SizeHorCursor);
            _direction = 6;
        }
        else {
            this->setCursor(Qt::ArrowCursor);
            _direction = 5;
        }
    }
    if(_onLeftMouseBtn) {
        this->setCursor(Qt::ClosedHandCursor);
        _currentPoint = QCursor::pos();
        if(5 == _direction) {
            _move();
        }
        else {
            _resize(_direction);
        }
    }
}

void CoverLabel::mouseReleaseEvent(QMouseEvent *event) {
    if(_onLeftMouseBtn) {
        _onLeftMouseBtn = false;
        _currentX += _x;
        _currentY += _y;
        _currentWidth = _width;
        _currentHeight = _height;
    }

}

void CoverLabel::_move() {
    int moveX = _currentPoint.x() - _startPoint.x();
    int moveY = _currentPoint.y() - _startPoint.y();
    _x = moveX;
    _y = moveY;
    //两对角判断是否出界
    if((_currentX+_x)<0) {
        _x = 0 - _currentX;
    }
    if((_currentY+_y)<0) {
        _y = 0 - _currentY;
    }
    if((_currentX+_x+_currentWidth) > _partnerWidth) {
        _x = _partnerWidth - _currentX- _currentWidth;
    }
    if((_currentY+_y+_currentHeight) > _partnerHeight) {
        _y = _partnerHeight - _currentY - _currentHeight;
    }
    this->move(_partnerX+_currentX+_x,_partnerY+_currentY+_y);
}


void CoverLabel::_resize(int n) {
    _x = 0;
    _y = 0;
    _width = _currentWidth;
    _height = _currentHeight;
    //从按钮点击移动矢量
    int moveX = _currentPoint.x() - _startPoint.x();
    int moveY = _currentPoint.y() - _startPoint.y();
    switch (n) {
        case 1:
            _x += moveX;
            _width -= moveX;
            _height += moveY;
            break;
        case 2:
            _height += moveY;
            break;
        case 3:
            _width += moveX;
            _height += moveY;
            break;
        case 4:
            _x += moveX;
            _width -= moveX;
            break;
        case 6:
            _width += moveX;
            break;
        case 7:
            _x += moveX;
            _y += moveY;
            _width -= moveX;
            _height -= moveY;
            break;
        case 8:
            _y += moveY;
            _height -= moveY;
            break;
        case 9:
            _y += moveY;
            _width += moveX;
            _height -= moveY;
        break;
        default:
        break;
    }
    // qDebug()<<"Before   _x: "<<_x<<" _y: "<<_y<<" _width: "<<_width<<" _height: "<<_height;
    //防止右上角变成左下角，恢复点击之前
    if(_width <=0 || _height<=0) {
        _onLeftMouseBtn = false;
        this->resize(_currentWidth,_currentHeight);
        this->move(_partnerX+_currentX,_partnerY+_currentY);
        return;
    }
    int m = 0;
    if((m = _currentX+_x)<0) {
        _x = 0 - _currentX;
        _width += m;
    }
    if((m = _currentY+_y)<0) {
        _y = 0 - _currentY;
        _height += m;
    }
    if((m = _currentX+_x+_width)>_partnerWidth) {
        _width = _partnerWidth - _currentX - _x;
    }
    if((m = _currentY+_y+_height)>_partnerHeight) {
        _height = _partnerHeight - _currentY - _y;
    }
    //防止两个对角超界
    this->resize(_width,_height);
    this->move(_x+_partnerX+_currentX,_y+_partnerY+_currentY);
    // qDebug()<<"partnerX: "<<_partnerX<<" partnerY: "<<_partnerY<<" currentX: "<<_currentX<<" currentY: "<<_currentY;
    // qDebug()<<"Before  _x: "<<_x<<" _y: "<<_y<<" _width: "<<_width<<" _height: "<<_height;
}
