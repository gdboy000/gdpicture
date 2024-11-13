//
// Created by gao on 24-11-11.
//

#include <QPixmap>
#include <QMouseEvent>

#include "canvaslabel.h"
#include "coverlabel.h"

#define SHOW(str) qDebug()<<str
CanvasLabel::CanvasLabel(QWidget *parent) : QLabel(parent)
{
    _mask = new CoverLabel(parent);
    _mask->setStyleSheet("background-color: rgba(255, 255, 255, 150);"
                         "border:3px dotted black");
}

CanvasLabel::~CanvasLabel() {

}

/**
 * set file
 * reload file path
 * record the percentage if the width and height reach the label edge
 * @param path
 * @return
 */
bool CanvasLabel::ShowImage(const QString &path) {
    if(path != _filePath) {
        _filePath = path;
        _image = QImage(path);
        if(_image.isNull()) return false;
        _imageWidth = _image.size().width();
        _imageHeight = _image.size().height();
        _ratio = _imageWidth/_imageHeight;
        if(_imageWidth > _imageHeight) {
            _vertical = true;
            _limitMaxZoomRatio = _parentSize.width()/static_cast<double>(_image.width());
        }
        else {
            _limitMaxZoomRatio = _parentSize.height()/static_cast<double>(_image.height());
        }
        return _layoyt();
    }
    return false;
}

void CanvasLabel::Zoom(int n) {
    double span = n>0?_pulleySpan:-_pulleySpan;
    if((_currentZoomRatio + span) > 0) {
        _layoyt(_currentZoomRatio + span);
    }
}


void CanvasLabel::SizeChanged(QSize size) {
    _parentSize = size;
    if(_vertical) {
        _limitMaxZoomRatio = _parentSize.width()/static_cast<double>(_image.width());
    }
    else {
        _limitMaxZoomRatio = _parentSize.height()/static_cast<double>(_image.height());
    }
    _layoyt(_currentZoomRatio);
}

void CanvasLabel::mouseMoveEvent(QMouseEvent *event) {
    //qDebug()<<"move";
    if(_rigthMouseClicked) {
         _currentPoint = QCursor::pos();
        _move();
    }

}

void CanvasLabel::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        _rigthMouseClicked = true;
        // qDebug()<<"right clicked";
        // qDebug()<<QCursor::pos();
        _startPoint = QCursor::pos(); // 记录点击位置
        event->accept();
    } else {
        QWidget::mousePressEvent(event);
    }
}

void CanvasLabel::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        _rigthMouseClicked = false;
        _x = this->pos().x();
        _y = this->pos().y();
    } else {
        QWidget::mousePressEvent(event);
    }
}

bool CanvasLabel::_layoyt(double percentage) {
    if(_image.isNull()) {
        return false;
    }
    int suitable_width = 0;
    int suitable_height = 0;
    //
    if(0.00 == percentage || percentage == _currentZoomRatio) {
        if(_vertical) {
            suitable_width = _parentSize.width()*_defaultPercentage;
            _currentZoomRatio = suitable_width/_imageWidth;
            suitable_height = suitable_width/_ratio;
        }
        else {
            suitable_height = _parentSize.height()*_defaultPercentage;
            _currentZoomRatio = suitable_height/_imageHeight;
            suitable_width = suitable_height*_ratio;
        }
        _x = (_parentSize.width()-suitable_width)/2;
        _y = (_parentSize.height()-suitable_height)/2;
        _mask->SizeChanged(QSize(suitable_width,suitable_height));
        _mask->Move(_x,_y);
        _resizeLabel(suitable_width, suitable_height);
        this->move(_x,_y);
        return true;
    }

    suitable_width = _imageWidth*percentage;
    suitable_height = _imageHeight*percentage;
    _x += _currentZoomRatio*_imageWidth - suitable_width;
    _y += _currentZoomRatio*_imageHeight - suitable_height;
    _resizeLabel(suitable_width,suitable_height);
    _mask->SizeChanged(QSize(suitable_width,suitable_height));
    _mask->Move(_x,_y);
    _mask->Zoom(_currentZoomRatio,percentage);
    this->move(_x,_y);
    _currentZoomRatio = percentage;
    // if(percentage < _currentZoomRatio) {
    //     // _x = (_parentSize.width()-suitable_width)/2;
    //     // _y = (_parentSize.height()-suitable_height)/2;
    //     // _resizeLabel(suitable_width,suitable_height);
    //     // this->move(_x+_offsetX,_y+_offsetY);
    //
    // }
    // else {
    //     _x = (suitable_width - _parentSize.width())/2;
    //     _y = (suitable_height - _parentSize.height())/2;
    //     _resizeLabel(suitable_width,suitable_height);
    //     this->move(_offsetX-_x,_offsetY-_y);
    // }
    return true;
}

bool CanvasLabel::_resizeLabel(double ratio) {
    if(!_image.isNull()) {
        QSize image_size = _image.size();
        QSize suitable_size;
        double image_width = image_size.width();
        double image_height = image_size.height();

        if(image_size.width() > image_size.height()) {
            suitable_size.setWidth(_parentSize.width()*0.8);
            suitable_size.setHeight(suitable_size.width()*ratio);
        }
        else {
            suitable_size.setHeight(_parentSize.height()*0.8);
            suitable_size.setWidth(suitable_size.height()*ratio);
        }
        _currentZoomRatio = suitable_size.width()/image_width;
        this->setPixmap(QPixmap::fromImage(_image.scaled(suitable_size)));
        this->setGeometry((_parentSize.width()-suitable_size.width())/2,(_parentSize.height()-suitable_size.height())/2,suitable_size.width(),suitable_size.height());
        return true;
    }
    return false;
}

bool CanvasLabel::_resizeLabel(int width,int height) {
    if(_image.isNull()) return false;
    this->setPixmap(QPixmap::fromImage(_image.scaled(width,height)));
    this->resize(width,height);

    emit sendMessage(QString("%1  原分辨率:%2x%3 缩放尺度:%4 ").arg(_filePath).arg(_image.width()).arg(_image.height()).arg(QString::number(_currentZoomRatio, 'f', 2)));//减少一下文件目录长度
    return true;
}

void CanvasLabel::_move() {
    int moveX = _currentPoint.x() - _startPoint.x();
    int moveY = _currentPoint.y() - _startPoint.y();
    //qDebug()<<"start:"<<_startPoint<<" current:"<<_currentPoint;
    this->move(_x+moveX,_y+moveY);
    _offsetX += moveX;
    _offsetY += moveY;
    // _x += moveX;
    // _y += moveY;
}

void CanvasLabel::_startMove() {

}

void CanvasLabel::_stopMove() {

}




