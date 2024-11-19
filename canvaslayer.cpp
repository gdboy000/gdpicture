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

/**
 * 设置画布层的图像
 * 初始化关于原始图像的三个变量，_prototypeImage,_prototypeImageSize,_prototypeImageAspectRatio
 * @param image 图像
 */
void CanvasLayer::setImage(const QImage& image) {
    _prototypeImage = image;
    _prototypeImageSize = _prototypeImage.size();
    _prototypeImageAspectRatio = static_cast<double>(_prototypeImage.width())/static_cast<double>(_prototypeImage.height());
}

/**
 * 展示图像
 */
void CanvasLayer::showImage() {
   if(!_prototypeImage.isNull()) {
       _reLayout();
   }
}

/**
 * 设置画布层图像并展示
 * 等效于 setImage + showImage
 * @param image 图像
 */
void CanvasLayer::showImage(const QImage& image) {
    _prototypeImage = image;
    _prototypeImageSize = _prototypeImage.size();
    _prototypeImageAspectRatio = static_cast<double>(_prototypeImage.width())/static_cast<double>(_prototypeImage.height());
    _reLayout();
}

/**
 * 设置上一层大小
 * 通常在上一层resize使用，效果是恢复默认布局
 * @param size 上一层大小
 */
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

/**
 * 重新布局
 * 该函数通常在缩放影响下使用，包括根据上一层大小设置默认缩放比率和根据缩放比率进行重新布局
 * @param zoomLevel 缩放比率
 * @return 结果
 */
bool CanvasLayer::_reLayout(double zoomLevel) {
    if(_prototypeImage.isNull()) return false;
    static double tmp1 = 0;
    static double tmp2 = 0;
    QSizeF suitable_size;//合适分辨率
    //默认layout，只会在初次默认展示使用
    if(0.00 == zoomLevel) {
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
        _currentImageSize = suitable_size;
    }
    else {
        suitable_size = QSize(_prototypeImage.width()*zoomLevel,_prototypeImage.height()*zoomLevel);
        double px,py;
        //原地放大，偏移恢复
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

/**
 * 改变当前层大小
 * 附加改变图像尺寸，传输当前缩放尺度信息给上一层
 * @param width resize宽度
 * @param height resize高度
 * @return 返回结果
 */
bool CanvasLayer::_resizeLabel(int width,int height) {
    if(_prototypeImage.isNull()) return false;
    this->setPixmap(QPixmap::fromImage(_prototypeImage.scaled(width,height)));
    this->resize(width,height);
    emit zoomRatioChanges(_currentZoomRatio);
    return true;
}

/**
 * 当前图层移动
 */
void CanvasLayer::_move() {
    int moveX = _currentPoint.x() - _startPoint.x();
    int moveY = _currentPoint.y() - _startPoint.y();
    this->move(_x+moveX,_y+moveY);
    _offsetX = moveX;
    _offsetY = moveY;
}






