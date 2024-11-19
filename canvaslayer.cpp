//
// Created by gao on 24-11-11.
//

#include <QPixmap>
#include <QMouseEvent>

#include "canvaslayer.h"


#define SHOW(str) qDebug()<<str
CanvasLayer::CanvasLayer(QWidget *parent) : CLayer(parent)
{
    setObjectName("CanvasLayer");
}

CanvasLayer::~CanvasLayer() {

}


void CanvasLayer::setImage(const QImage& image) {
    _prototypeImage = image;
    _prototypeImageSize = _prototypeImage.size();
    _prototypeImageAspectRatio = static_cast<double>(_prototypeImage.width())/static_cast<double>(_prototypeImage.height());
}

void CanvasLayer::showImage() {
   if(!_prototypeImage.isNull()) {
       _reLayout();
   }
}

void CanvasLayer::showImage(const QImage& image) {
    _prototypeImage = image;
    _prototypeImageSize = _prototypeImage.size();
    _prototypeImageAspectRatio = static_cast<double>(_prototypeImage.width())/static_cast<double>(_prototypeImage.height());
    _reLayout();
}

void CanvasLayer::Zoom(int n) {
    double span = n>0?_pulleySpan:-_pulleySpan;
    if((_currentZoomRatio + span) > 0) {
        _reLayout(_currentZoomRatio + span);
    }
}

void CanvasLayer::setHigherSize(const QSize& size) {
    _offsetX = 0;
    _offsetY = 0;
    _currentParentSize = size;
}

/**
 * 鼠标移动事件重写，只有在右键点击状态下才会拦截(这部分可以使用button判断是否还在点击)，其他向上传递。
 * 获取鼠标当前全局位置，因为要保证鼠标在移动到窗口外还监听到。
 * 修改当前鼠标位置_currentPoint，交给_move使用。
 * 注意取消刚开始的鼠标跟踪。
 * @param event 鼠标事件
 */
void CanvasLayer::mouseMoveEvent(QMouseEvent *event) {
    if(_rigthMouseClicked) {
         _currentPoint = QCursor::pos();
        _move();
        event->accept();
    }
    else {
        qDebug()<<"鼠标移动传递";
        event->ignore();
    }
}

/**
 * 鼠标点击事件，只拦截鼠标右键点击事件。
 * 设置右键点击状态，保存开始位置。
 * 初始化一段点击和结束的偏移量为0。
 * @param event 鼠标事件
 */
void CanvasLayer::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        _rigthMouseClicked = true;
        _startPoint = QCursor::pos(); // 记录点击位置
        _offsetX = 0;
        _offsetY = 0;
        event->accept();
    } else {
        event->ignore();
    }
}

/**
 * 鼠标释放事件，只拦截鼠标右键的释放事件。
 * 设置右键释放状态，记录当前控件位置。
 * 记录当前位置较之于之前位置的偏移量。
 * @param event 鼠标事件
 */
void CanvasLayer::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        _rigthMouseClicked = false;
        _x = this->pos().x();
        _y = this->pos().y();
        _currentOffsetX += _offsetX;
        _currentOffsetY += _offsetY;
        event->accept();
    } else {
        event->ignore();
    }
}

/**
 * 滚轮事件重写，注意只拦截ctrl+滚轮事件。
 * 获取滚轮滚动量X尺度，传给_reLayout重新布局。
 * @param event 滚轮事件
 */
void CanvasLayer::wheelEvent(QWheelEvent *event) {
    int n = event->angleDelta().y();
    if (event->modifiers() & Qt::ControlModifier) {
        double span = n>0?_pulleySpan:-_pulleySpan;
        if((_currentZoomRatio + span) > 0) {
            _reLayout(_currentZoomRatio + span);
        }
    }
    else {
        QWidget::wheelEvent(event);
    }
}

void CanvasLayer::moveEvent(QMoveEvent *event) {
    emit moved(event->pos());
    QWidget::moveEvent(event);
}

void CanvasLayer::resizeEvent(QResizeEvent *event) {
    emit resized(event->size());
    QWidget::resizeEvent(event);
}


bool CanvasLayer::_reLayout(double zoomLevel) {
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
        tmp1 = suitable_size.toSize().width()/2+_x;
        tmp2 = suitable_size.toSize().height()/2+_y;
        _vectorDirection = {_currentParentSize.width()/2-_x,_currentParentSize.height()/2-_y};
        _currentImageSize = suitable_size;
    }
    else {
        suitable_size = QSize(_prototypeImage.width()*zoomLevel,_prototypeImage.height()*zoomLevel);
        double px,py;
        if(_offsetX==0 && _offsetY==0) {
            _x += (_currentZoomRatio*_prototypeImage.width() - suitable_size.toSize().width())/2;
            _y += (_currentZoomRatio*_prototypeImage.height() - suitable_size.toSize().height())/2;
            _x = qRound(_x);
            _y = qRound(_y);
            px = qRound(tmp1) - qRound(suitable_size.width()/2+_x);
            py = qRound(tmp2) - qRound(suitable_size.height()/2+_y);
            _x += px;
            _y += py;
        }
        _currentImageSize = suitable_size;
        _currentZoomRatio = zoomLevel;
    }
    _resizeLabel(suitable_size.toSize());
    this->move(_x,_y);
    return true;
}

bool CanvasLayer::_resizeLabel(double ratio) {
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

bool CanvasLayer::_resizeLabel(int width,int height) {
    if(_prototypeImage.isNull()) return false;
    this->setPixmap(QPixmap::fromImage(_prototypeImage.scaled(width,height)));
    this->resize(width,height);
    emit sendMessage(QString("%1  原分辨率:%2x%3 缩放尺度:%4 ").arg(_filePath).arg(_prototypeImage.width()).arg(_prototypeImage.height()).arg(QString::number(_currentZoomRatio, 'f', 2)));//减少一下文件目录长度
    return true;
}

void CanvasLayer::_move() {
    int moveX = _currentPoint.x() - _startPoint.x();
    int moveY = _currentPoint.y() - _startPoint.y();
    // if(nullptr != _mask) _mask->Move(_x+moveX,_y+moveY);
    this->move(_x+moveX,_y+moveY);
    _offsetX = moveX;
    _offsetY = moveY;
}






