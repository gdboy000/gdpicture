//
// Created by gao on 24-11-11.
//


#include "showwidget.h"
#include "coverlayer.h"
#include "canvaslayer.h"

#include <QMouseEvent>
#include <QCursor>



ShowWidget::ShowWidget(QWidget *parent) :
    QWidget(parent) {
    _canvasLayer = new CanvasLayer(this);
    _canvasLayer->hide();
    _initConnect();

}

ShowWidget::~ShowWidget() {

}

/**
 * 打开图像文件名，并显示
 * @param fileName 文件名
 * @return 图像是否显示成功
 */
bool ShowWidget::openFile(const QString &fileName) {
    _showImage = QImage(fileName);
    if (_showImage.isNull()) {
        return false;
    }
    setMouseTracking(true);
    _canvasLayer->setHigherSize(this->size());
    _canvasLayer->showImage(_showImage);
    _canvasLayer->show();
    return true;
}

/**
 * 初始化裁功能
 * 也就是初始化遮盖层
 */
void ShowWidget::initCutMode() {
    if(nullptr != _coverLayer) {
        delete _coverLayer;
    }
    _coverLayer = new CoverLayer(this);
    _coverLayer->setStyleSheet("background-color: rgba(255, 255, 255, 150);"
                         "border:1px dotted black");
    _coverLayer->setRestrictedLayer(_canvasLayer);
    _coverLayer->show();
}

/**
 * 当前控件大小改变
 * @param event 大小改变事件
 */
void ShowWidget::resizeEvent(QResizeEvent *event) {
    _canvasLayer->setHigherSize(this->size());
    _canvasLayer->showImage();
    QWidget::resizeEvent(event);
}

void ShowWidget::mouseMoveEvent(QMouseEvent *event) {
    static int n = 0;
    //不松开n不变,要不然过快的拖拉速度，跟不上
    if(nullptr != _coverLayer && !_leftMouseClicked) {
        n = _coverLayerDirectionJudge(event->pos());
    }
    //因为涉及超出当前窗口，所以以QCursor::pos();来计算
    else if(_leftMouseClicked) {
        if(_layer->objectName() == "CoverLayer") {
            if(0 != n) {
                _coverLayer->setOffset(QCursor::pos() - _oldPos,n);
            }
            else {
                //_leftMouseClicked = false;
            }
            _oldPos = QCursor::pos();
        }
    }
    event->accept();
}

void ShowWidget::mousePressEvent(QMouseEvent *event) {
    //左键点击事件
    if (event->button() == Qt::LeftButton) {
        //判断是否在遮盖层
        if(nullptr!=_coverLayer ) {
            QRect rect = QRect(_coverLayer->pos()+QPoint(-2,-2),_coverLayer->size()+QSize(4,4));
            if(rect.contains(event->pos())){
                _layer = _coverLayer;
                _startClickedPoint = QCursor::pos();
                _oldPos = _startClickedPoint;
                _leftMouseClicked = true;
                event->accept();
                return;
            }

        }
    }
    QWidget::mousePressEvent(event);
}

void ShowWidget::mouseReleaseEvent(QMouseEvent *event) {
    //左键释放事件
    if (event->button() == Qt::LeftButton) {
        _leftMouseClicked = false;
        event->accept();
    }
    //右键释放事件
    else if(event->button() == Qt::RightButton) {
        _rightMouseClicked = false;
        event->accept();
    }
    else {
        QWidget::mousePressEvent(event);
    }
}


void ShowWidget::_initConnect() {
    connect(_canvasLayer,&CanvasLayer::zoomRatioChanges,[this](double ratio) {
        emit statusMessageChanges(QString(" 分辨率:%1X%2 缩放比率:%3").arg(_showImage.width()).arg(_showImage.height()).arg(QString::number(ratio, 'f', 2)));
    });
}

/**
 * 遮盖层鼠标位置判断和样式设置
 * @param pos 鼠标位置
 * @return
 */
int ShowWidget::_coverLayerDirectionJudge(const QPoint &pos) {
    static int thickness = 10;
    static int oThickness = 2;//4
    static int iThickness = 8;//2
    //遮盖层9个区域
    if(nullptr == _coverLayer) return 0;
    QPoint point = _coverLayer->pos();
    int width = _coverLayer->size().width();
    int height = _coverLayer->size().height();
    QRect direction7(point+QPoint(-oThickness,-oThickness),QSize(thickness,thickness));
    QRect direction8(point+QPoint(iThickness,-oThickness),QSize(width-iThickness*2,thickness));
    QRect direction9(point+QPoint(width-iThickness,-oThickness),QSize(thickness,thickness));

    QRect direction4(point+QPoint(-oThickness,iThickness),QSize(thickness,height-iThickness*2));
    QRect direction5(point+QPoint(iThickness,iThickness),QSize(width-iThickness*2,height-iThickness*2));
    QRect direction6(point+QPoint(width-iThickness,iThickness),QSize(thickness,height-iThickness*2));

    QRect direction1(point+QPoint(-oThickness,height-iThickness),QSize(thickness,thickness));
    QRect direction2(point+QPoint(iThickness,height-iThickness),QSize(width-iThickness*2,thickness));
    QRect direction3(point+QPoint(width-iThickness,height-iThickness),QSize(thickness,thickness));
    if(direction1.contains(pos)) {
        this->setCursor(Qt::SizeBDiagCursor);
        return 1;
    }
    if(direction2.contains(pos)) {
        this->setCursor(Qt::SizeVerCursor);
        return 2;
    }
    if(direction3.contains(pos)) {
        this->setCursor(Qt::SizeFDiagCursor);
        return 3;
    }
    if(direction4.contains(pos)) {
        this->setCursor(Qt::SizeHorCursor);
        return 4;
    }
    if(direction5.contains(pos)) {
        this->setCursor(Qt::ArrowCursor);
        return 5;
    }
    if(direction6.contains(pos)) {
        this->setCursor(Qt::SizeHorCursor);
        return 6;
    }
    if(direction7.contains(pos)) {
        this->setCursor(Qt::SizeFDiagCursor);
        return 7;
    }
    if(direction8.contains(pos)) {
        this->setCursor(Qt::SizeVerCursor);
        return 8;
    }
    if(direction9.contains(pos)) {
        this->setCursor(Qt::SizeBDiagCursor);
        return 9;
    }
    return 0;
}


