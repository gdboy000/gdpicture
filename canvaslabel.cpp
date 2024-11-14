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
    _mask->hide();
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
        _prototypeImage = QImage(path);
        if(_prototypeImage.isNull()) return false;
        _prototypeImageSize = _prototypeImage.size();
        _prototypeImageAspectRatio = static_cast<double>(_prototypeImage.width())/static_cast<double>(_prototypeImage.height());
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
        return _reLayout();
    }
    return false;
}

void CanvasLabel::Zoom(int n) {
    double span = n>0?_pulleySpan:-_pulleySpan;
    if((_currentZoomRatio + span) > 0) {
        _reLayout(_currentZoomRatio + span);
    }
}

void CanvasLabel::SizeChanged(QSize size) {
    _defaultPercentage = 1.00 - (30.00 / size.width());
    _currentParentSize = size;
    if(_vertical) {
        _limitMaxZoomRatio = _currentParentSize.width()/static_cast<double>(_image.width());
    }
    else {
        _limitMaxZoomRatio = _currentParentSize.height()/static_cast<double>(_image.height());
    }
    _reLayout();
}

void CanvasLabel::mouseMoveEvent(QMouseEvent *event) {
    if(_rigthMouseClicked) {
         _currentPoint = QCursor::pos();
        _move();
    }

}

void CanvasLabel::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        _rigthMouseClicked = true;
        _startPoint = QCursor::pos(); // 记录点击位置
        _offsetX = 0;
        _offsetY = 0;
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
        _currentOffsetX += _offsetX;
        _currentOffsetY += _offsetY;
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
            suitable_width = _currentParentSize.width()*_defaultPercentage;
            _currentZoomRatio = suitable_width/_imageWidth;
            suitable_height = suitable_width/_ratio;
            if(suitable_height > _currentParentSize.height()) {
                _currentZoomRatio = _currentParentSize.height()/_imageHeight;
                suitable_height =  _currentParentSize.height();
                suitable_width = suitable_height*_ratio;
            }
        }
        else {
            suitable_height = _currentParentSize.height()*_defaultPercentage;
            _currentZoomRatio = suitable_height/_imageHeight;
            suitable_width = suitable_height*_ratio;
            if(suitable_width > _currentParentSize.width()) {
                _currentZoomRatio = _currentParentSize.width()/_imageWidth;
                suitable_width = _currentParentSize.width();
                suitable_height = suitable_width/_ratio;
            }
        }
        if(0.00 == percentage || (_parentSize.width()<0 && _parentSize.height()<0)) {
            _x = (_currentParentSize.width()-suitable_width)/2;
            _y = (_currentParentSize.height()-suitable_height)/2;
        }
        else {
            _x += (_currentParentSize.width() - _parentSize.width())/2;
            _y += (_currentParentSize.height() - _parentSize.height())/2;
        }
        //qDebug()<<" "<<_currentParentSize<<" "<<_parentSize;
        _parentSize = _currentParentSize.toSize();
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
    return true;
}

bool CanvasLabel::_reLayout(double zoomLevel) {
    if(_prototypeImage.isNull()) return false;
    QSizeF suitable_size;//合适分辨率
    //默认layout
    if(0.00 == zoomLevel) {
        //依赖height
        if((_currentParentSize.width()/_prototypeImage.width())*_prototypeImage.height() > _currentParentSize.height()) {
            suitable_size.setHeight(_currentParentSize.height()*_defaultPercentage);
            suitable_size.setWidth(suitable_size.height()*_prototypeImageAspectRatio);
            _currentZoomRatio = suitable_size.height()/_prototypeImage.height();
        }
        else {
            suitable_size.setWidth(_currentParentSize.width()*_defaultPercentage);
            suitable_size.setHeight(suitable_size.width()/_prototypeImageAspectRatio);
            _currentZoomRatio = suitable_size.width()/_prototypeImage.width();
        }
        _x = (_currentParentSize.width()-suitable_size.width())/2;
        _y = (_currentParentSize.height()-suitable_size.height())/2;
        _vectorDirection = {_currentParentSize.width()/2-_x,_currentParentSize.height()/2-_y};
        _currentImageSize = suitable_size;
        //原中心点
        tmp1 = suitable_size.toSize().width()/2+_x;
        tmp2 = suitable_size.toSize().height()/2+_y;
        qDebug()<<"中心点改变："<<tmp1<<","<<tmp2;
    }
    else {
        suitable_size = QSize(_prototypeImage.width()*zoomLevel,_prototypeImage.height()*zoomLevel);
        //(当前的缩放比例*原型图像分辨率 - 需要的分辨率)/2


        // int x = qRound(std::abs(_currentOffsetX)*(zoomLevel/_currentZoomRatio)+_currentImageSize.width()/2+_x);
        // int y = qRound(std::abs(-_currentOffsetY)*(zoomLevel/_currentZoomRatio)+_currentImageSize.height()/2+_y);
        // qDebug()<<"拖动后中心点："<<x<<","<<y;
        //qDebug()<<"移动后窗口大小："<<_currentParentSize<<" 适应后图像大小："<<suitable_size<<" 移动后位置："<<_x<<","<<_y;

        //qDebug()<<"原来中心位置："<<tmp1<<","<<tmp2<<"  现在中心位置："<<_x+suitable_size.toSize().width()/2<<","<<_y+suitable_size.toSize().height()/2;
        // double px = qRound(tmp1) - qRound(suitable_size.width()/2+_x);
        // double py = qRound(tmp2) - qRound(suitable_size.height()/2+_y);
        double px,py;
        // if(_offsetX<0 && _offsetY<0) {
        //     px = qRound(tmp1) - qRound(std::abs(_currentOffsetX)*(zoomLevel/_currentZoomRatio)+_currentImageSize.width()/2+_x);;
        //     py = qRound(tmp2) - qRound(std::abs(-_currentOffsetY)*(zoomLevel/_currentZoomRatio)+_currentImageSize.height()/2+_y);
        // }
        // else if(_offsetX<0 && _offsetY>=0) {
        //     px = qRound(tmp1) - qRound(std::abs(_currentOffsetX)*(zoomLevel/_currentZoomRatio)+_currentImageSize.width()/2+_x);
        //     py = qRound(tmp2) - qRound(_currentParentSize.height()-std::abs(-_currentOffsetY)*(zoomLevel/_currentZoomRatio)-_currentImageSize.height()/2-std::abs(suitable_size.height()-_currentParentSize.height()+_y));
        // }
        // else if(_offsetX>=0 && _offsetY<0) {
        //     px = qRound(tmp1) - qRound(_currentParentSize.width()-std::abs(_currentOffsetX)*(zoomLevel/_currentZoomRatio)-_currentImageSize.width()/2-std::abs(suitable_size.width()-_currentParentSize.width()+_x));
        //     py = qRound(tmp2) - qRound(std::abs(-_currentOffsetY)*(zoomLevel/_currentZoomRatio)+_currentImageSize.height()/2+_y);
        // }
        // else if((_offsetX>=0 && _offsetY>0) || _offsetX>0 && _offsetY>=0) {
        //     px = qRound(tmp1) - qRound(_currentParentSize.width()-std::abs(_currentOffsetX)*(zoomLevel/_currentZoomRatio)-_currentImageSize.width()/2-std::abs(suitable_size.width()-_currentParentSize.width()+_x));
        //     py = qRound(tmp2) - qRound(_currentParentSize.height()-std::abs(-_currentOffsetY)*(zoomLevel/_currentZoomRatio)-_currentImageSize.height()/2-std::abs(suitable_size.height()-_currentParentSize.height()+_y));
        //     qDebug()<<"右下中心位置"<<qRound(_currentParentSize.width()-std::abs(_currentOffsetX)*(zoomLevel/_currentZoomRatio)-_currentImageSize.width()/2-std::abs(suitable_size.width()-_currentParentSize.width()+_x))<<qRound(_currentParentSize.height()-std::abs(-_currentOffsetY)*(zoomLevel/_currentZoomRatio)-_currentImageSize.height()/2-std::abs(suitable_size.height()-_currentParentSize.height()+_y));
        //     qDebug()<<px<<","<<py<<" _x,_y: "<<_x<<","<<_y;
        // }
        if(_offsetX==0 && _offsetY==0) {
            _x += (_currentZoomRatio*_prototypeImage.width() - suitable_size.toSize().width())/2;
            _y += (_currentZoomRatio*_prototypeImage.height() - suitable_size.toSize().height())/2;
            _x = qRound(_x);
            _y = qRound(_y);
            px = qRound(tmp1) - qRound(suitable_size.width()/2+_x);
            py = qRound(tmp2) - qRound(suitable_size.height()/2+_y);
            //qDebug()<<"偏移："<<qRound(suitable_size.width()/2+_x)<<","<<qRound(suitable_size.height()/2+_y);
            _x += px;
            _y += py;
        }

        //qDebug()<<"偏移："<<px<<","<<py;

        // qDebug()<<"\n原来在图像上中心点："<<tmp1<<","<<tmp2<<" 现在中心点位置："<<px<<","<<py;
        // qDebug()<<"原来图像大小："<<_currentImageSize<<" 正确中心位置："<<_currentImageSize.width()/2<<","<<_currentImageSize.height()/2;
        // qDebug()<<"现在图像大小："<<suitable_size<<" 正确中心位置："<<suitable_size.width()/2<<","<<suitable_size.height()/2;
        // double x = -(tmp1*zoomLevel/_currentZoomRatio - _currentParentSize.width()/2);
        // double y = -(tmp2*zoomLevel/_currentZoomRatio - _currentParentSize.height()/2);
        // tmp1 = int(_currentParentSize.width()/2 - x);
        // tmp2 = int(_currentParentSize.height()/2 - y);
        //qDebug()<<"现实中心位置："<<qRound(_x)+suitable_size.toSize().width()/2<<","<<qRound(_y)+suitable_size.toSize().height()/2<<" 正常的："<<tmp1<<","<<tmp2;
        // qDebug()<<"_x: "<<_x<<" _y: "<<_y<<" x: "<<x<<" y: "<<y;
        _currentImageSize = suitable_size;
        _currentZoomRatio = zoomLevel;
    }
    _resizeLabel(suitable_size.toSize());
    this->move(_x,_y);
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
    if(_prototypeImage.isNull()) return false;
    this->setPixmap(QPixmap::fromImage(_prototypeImage.scaled(width,height)));
    this->resize(width,height);
    emit sendMessage(QString("%1  原分辨率:%2x%3 缩放尺度:%4 ").arg(_filePath).arg(_prototypeImage.width()).arg(_prototypeImage.height()).arg(QString::number(_currentZoomRatio, 'f', 2)));//减少一下文件目录长度
    return true;
}

void CanvasLabel::_move() {
    int moveX = _currentPoint.x() - _startPoint.x();
    int moveY = _currentPoint.y() - _startPoint.y();
    this->move(_x+moveX,_y+moveY);
    // tmp1 = _currentImageSize.toSize().width()/2+_x+moveX;
    // tmp2 = _currentImageSize.toSize().height()/2+_y+moveY;
    // qDebug()<<"中心点改变："<<tmp1<<","<<tmp2;
    _offsetX = moveX;
    _offsetY = moveY;
}

void CanvasLabel::_startMove() {

}

void CanvasLabel::_stopMove() {

}




