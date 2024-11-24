//
// Created by gao on 24-11-12.
//
#include <QPaintEvent>
#include <QPainter>

#include "coverlayer.h"

CoverLayer::CoverLayer(QWidget *parent):
    CLayer(parent){
    _x = 0;
    _y = 0;
    _currentX = 0;
    _currentY = 0;
    _currentWidth = 0;
    _currentHeight = 0;
    _partnerWidth = 0;
    _partnerHeight = 0;
    setObjectName("CoverLayer");
    //启用鼠标跟踪，如果不启用，在父控件中无法响应
    setMouseTracking(true);
}

CoverLayer::~CoverLayer() {

}

/**
 * 设置受限制层，来初始化默认布局
 * 注意可能将来需要重写个传QWidget的
 * 初始化成员，布局layer位置，建立与受限制层的连接
 * @param layer 层
 */
void CoverLayer::setRestrictedLayer(const CLayer* layer) {
    this->move(layer->pos());
    this->resize(layer->size());
    _currentX = 0;
    _currentY = 0;
    _partnerX = layer->pos().x();
    _partnerY = layer->pos().y();
    _partnerWidth = _currentWidth = layer->size().width();
    _partnerHeight = _currentHeight = layer->size().height();
    _memPosition.clear();
    _memPartnerSize.clear();
    _memPosition.push_back(QRect(_currentX,_currentY,_currentWidth,_currentHeight));
    _memPartnerSize.push_back(QSize(_partnerWidth,_partnerHeight));
    _tmpPosition = {0,0,0,0};
    connect(layer,&CLayer::moved,this,&CoverLayer::effectByPosOfLayer);
    connect(layer,&CLayer::resized,this,&CoverLayer::effectBySizeOfLayer);
}

void CoverLayer::setDefaultLayer() {
    this->resize(_partnerWidth,_partnerHeight);
    this->move(_partnerX,_partnerY);
    _currentX = 0;
    _currentY = 0;
    _currentWidth = _partnerWidth;
    _currentHeight = _partnerHeight;
    _memPosition.clear();
    _memPartnerSize.clear();
    _memPosition.push_back(QRect(_currentX,_currentY,_currentWidth,_currentHeight));
    _memPartnerSize.push_back(QSize(_partnerWidth,_partnerHeight));
    _tmpPosition = {0,0,0,0};
}


/**
 * 收到限制层大小，重新布局
 * @param size 限制层的大小
 */
void CoverLayer::effectBySizeOfLayer(const QSize &size) {
    //等比例操作 原图横宽/原伙伴横宽 == 现在横宽/现在伙伴横宽
    const double ratio = size.width()/_partnerWidth;
    //等比例size
    _currentWidth = qRound(_currentWidth*ratio);
    _currentHeight = qRound(_currentHeight*ratio);
    //等比例position
    _currentX = qRound(_currentX*ratio);
    _currentY = qRound(_currentY*ratio);
    _partnerWidth = size.width();
    _partnerHeight = size.height();
    this->resize(_currentWidth,_currentHeight);
}

/**
 * 收到受限制层位置，改变布局
 * @param pos 限制层位置
 */
void CoverLayer::effectByPosOfLayer(const QPoint& pos) {
    _partnerX = pos.x();
    _partnerY = pos.y();
    this->move(_currentX+_partnerX,_currentY+_partnerY);
}

/**
 * 偏移设置
 * 注意该偏移是每次move都需要刷新当前位置
 * @param point 偏移大小
 * @param n 偏移方向
 */
void CoverLayer::setOffset(const QPoint &point,int n) {
    _x = 0;
    _y = 0;
    _width = _currentWidth;
    _height = _currentHeight;
    //从按钮点击移动矢量
    int moveX = point.x();
    int moveY = point.y();
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
        case 5:
            _x += moveX;
            _y += moveY;
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
    //防止缩小过度，返回原样
    if(_width <=0 || _height<=0) {
        this->resize(_currentWidth,_currentHeight);
        this->move(_partnerX+_currentX,_partnerY+_currentY);
        return;
    }
    //防止两个对角超界
    if(_currentX+_x<0) {
        _x = 0 - _currentX;
        _width = _currentWidth;
    }
    if(_currentY+_y<0) {
        _y = 0 - _currentY;
        _height = _currentHeight;
    }
    if(_currentX+_x+_width>_partnerWidth) {
        _x = 0;
        _width = _currentWidth;
    }
    if(_currentY+_y+_height>_partnerHeight) {
        _y = 0;
        _height = _currentHeight;
    }

    this->resize(_width,_height);
    this->move(_x+_partnerX+_currentX,_y+_partnerY+_currentY);
    _currentWidth = _width;
    _currentHeight = _height;
    _currentX = _currentX + _x;
    _currentY = _currentY + _y;
}

void CoverLayer::moveEnd() {
    if(_tmpPosition.size() != QSize(0,0)) {
        _memPosition.push_back(_tmpPosition);
        _memPartnerSize.push_back(_tmpPartnerSize);
    }
    _tmpPosition = QRect(_currentX,_currentY,_currentWidth,_currentHeight);
    _tmpPartnerSize = QSize(_partnerWidth,_partnerHeight);
}

bool CoverLayer::moveBack() {
    QRect rect = _memPosition.back();
    QSize size = _memPartnerSize.back();
    //等比例操作 原图横宽/原伙伴横宽 == 现在横宽/现在伙伴横宽
    const double ratio = _partnerWidth/size.width();
    //等比例size
    rect.setWidth(qRound(rect.width()*ratio));
    rect.setHeight(qRound(rect.height()*ratio));
    //等比例position
    rect.setX(qRound(rect.x()*ratio));
    rect.setY(qRound(rect.y()*ratio));
    this->resize(rect.size());
    this->move(rect.x()+_partnerX,rect.y()+_partnerY);
    _currentWidth = rect.width();
    _currentHeight = rect.height();
    _currentX = rect.x();
    _currentY = rect.y();
    if(1 != _memPosition.size()) {
        _memPosition.pop_back();
        _memPartnerSize.pop_back();
        return false;
    }
    return true;
}


/**
 * 重新绘制边框，和内部样式
 * @param event 绘画事件
 */
void CoverLayer::paintEvent(QPaintEvent *event) {
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

