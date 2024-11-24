//
// Created by gao on 24-11-11.
//


#include "showwidget.h"
#include "coverlayer.h"
#include "canvaslayer.h"

#include <QMouseEvent>
#include <QCursor>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFile>



ShowWidget::ShowWidget(QWidget *parent) :
    QWidget(parent) {
    _canvasLayer = new CanvasLayer(this);
    _canvasLayer->hide();
    _initConnect();
    _initOption();
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
    _semiImage.clear();
    _semiSize.clear();
    _semiImage.push_back(_showImage);
    _semiSize.push_back(QRect(QPoint(0,0),_showImage.size()));
    _oldPos = {0,0};
    setMouseTracking(true);
    _canvasLayer->setHigherSize(this->size());
    _canvasLayer->showImage(_showImage);
    _canvasLayer->show();
    if(nullptr != _coverLayer) {
        _coverLayer->close();
        delete _coverLayer;
        _coverLayer = nullptr;
        _option->hide();
    }
    return true;
}

/**
 * 打开图像，并显示
 * @param image 图像
 * @return 图像是否显示成功
 */
bool ShowWidget::openImage(const QImage &image) {
    if (image.isNull()) {
        return false;
    }
    _oldPos = {0,0};
    setMouseTracking(true);
    _canvasLayer->setHigherSize(this->size());
    _canvasLayer->showImage(image);
    _canvasLayer->show();
    return true;
}

/**
 * 初始化裁功能
 * 也就是初始化遮盖层
 */
void ShowWidget::initCutMode() {
    if(nullptr != _coverLayer) {
        _coverLayer->setDefaultLayer();
        _coverLayer->show();
        _showOption(_coverLayer->pos()+QPoint(_coverLayer->width(),_coverLayer->height()));
        return;
    }
    _coverLayer = new CoverLayer(this);
    _coverLayer->setStyleSheet("background-color: rgba(255, 255, 255, 150);"
                         "border:1px dotted black");
    _coverLayer->setRestrictedLayer(_canvasLayer);
    _coverLayer->show();
    _showOption(_coverLayer->pos()+QPoint(_coverLayer->width(),_coverLayer->height()));
}

bool ShowWidget::saveCutRegion(const QString &path,QString& message) {
    QImage image = _semiImage.back();
    if(QFile(path).exists()) {
        message = QString("文件已存在");
        return false;
    }
    return image.save(path);
}



/**
 * 当前控件大小改变
 * @param event 大小改变事件
 */
void ShowWidget::resizeEvent(QResizeEvent *event) {
    _canvasLayer->setHigherSize(this->size());
    _canvasLayer->showImage();
    if(nullptr != _coverLayer) _showOption(pos());
    QWidget::resizeEvent(event);
}

void ShowWidget::mouseMoveEvent(QMouseEvent *event) {
    static int n = 0;
    //不松开n不变,要不然过快的拖拉速度，跟不上
    if(nullptr != _coverLayer && !_leftMouseClicked) {
        n = _coverLayerDirectionJudge(event->pos());
        if(0 == n) this->setCursor(Qt::ArrowCursor);
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
                _option->hide();
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
        if(_layer->objectName() == "CoverLayer") {
            _coverLayer->moveEnd();
            _showOption(event->pos());
            _layer = nullptr;
        }
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

/**
 * 初始化连接
 */
void ShowWidget::_initConnect() {
    connect(_canvasLayer,&CanvasLayer::zoomRatioChanges,[this](double ratio) {
        _ratio = ratio;
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

void ShowWidget::_initOption() {
    _option = new QWidget(this);
    _option->setStyleSheet("QWidget {border:1px solid rgba(0,0,0,0.5);border-radius:3px;} QWidget QPushButton{min-width:30px;min-height:33px;border:none;font-size:10px;} QWidget QPushButton:hover{border-radius:0px;}");
    QPushButton* confirmBtn = new QPushButton(_option);
    confirmBtn->setStyleSheet(":hover{background-color:green;}");
    confirmBtn->setText("确认");
    QPushButton* closeBtn = new QPushButton(_option);
    closeBtn->setText("关闭");
    closeBtn->setStyleSheet(":hover{background-color:red}");
    QPushButton* flushBtn = new QPushButton(_option);
    flushBtn->setStyleSheet(":hover{background-color:black;color:white;}");
    flushBtn->setText("刷新");
    QPushButton* backBtn = new QPushButton(_option);
    backBtn->setStyleSheet(":hover{background-color:rgb(153, 193, 241);}");
    backBtn->setText("返回");
    backBtn->hide();
    backBtn->setObjectName("back");
    _option->setLayout(new QHBoxLayout);
    _option->layout()->setContentsMargins(1,1,1,1);
    _option->layout()->addWidget(confirmBtn);
    _option->layout()->addWidget(closeBtn);
    _option->layout()->addWidget(flushBtn);
    _option->layout()->addWidget(backBtn);
    _option->setFixedSize(120,35);
    _option->layout()->setSpacing(0);
    _option->move(100,100);
    _option->hide();
    connect(confirmBtn,&QPushButton::clicked,[this]{_cutPartShow();});
    connect(closeBtn,&QPushButton::clicked,[this]{_coverLayer->hide();_backBtnHide();});
    connect(flushBtn,&QPushButton::clicked,[this]{_coverLayer->setDefaultLayer();});
    connect(backBtn,&QPushButton::clicked,[this] {
        if(_coverLayer->moveBack()) {
            _backBtnHide();
        }
    });
}

void ShowWidget::_showOption(const QPoint& point) {
    int x,y,width,height;
    width = 100;
    height = 35;
    if(point.x()+width/2+5 > this->width()) {
        x = this->width() - 5 - width;
    }
    else if(point.x()-width/2 < 5) x = 5;
    else {
        x = point.x() - width/2;
    }
    if(_oldPos != QPoint(0,0)) {
        _backBtnShow();
    }
    if(point.y()+height + 10> this->height()-5) y = this->height() - 5 - height - 10;
    else y = point.y();

    _option->move(this->width()-135,this->height()-_option->height()-5);
    _option->show();
}

void ShowWidget::_cutPartShow() {
    QPoint point = _coverLayer->getRelativePosition();
    int x = point.x();
    int y = point.y();
    int width = _coverLayer->width();
    int height = _coverLayer->height();

    QRect rect(qRound(x/_ratio),qRound(y/_ratio),qRound(width/_ratio),qRound(height/_ratio));
    QImage croppedImage = _showImage.copy(rect);
    if(openImage(croppedImage)) {
        _semiImage.push_back(croppedImage);
        _semiSize.push_back(rect);
        delete _coverLayer;
        _coverLayer = nullptr;
        _option->hide();
        _backBtnShow();
    }
}

void ShowWidget::_backBtnShow() {
    QPushButton* btn = this->_option->findChild<QPushButton*>("back");
    if(nullptr != btn) {
        btn->show();
        _option->setFixedSize(130,35);
    }
}

void ShowWidget::_backBtnHide() {
    QPushButton* btn = this->_option->findChild<QPushButton*>("back");
    if(nullptr != btn) {
        btn->hide();
        _option->setFixedSize(100,35);
    }
}




