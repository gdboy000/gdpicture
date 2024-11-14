//
// Created by gao on 24-11-11.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved
#include <QFileDialog>
#include <QScreen>
#include <QPushButton>
#include <QWheelEvent>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "canvaslabel.h"
#include "showwidget.h"

#define SHOW(str) qDebug()<<str

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), _ui(new Ui::MainWindow) {
    _ui->setupUi(this);
    const QScreen *pScreen = QGuiApplication::primaryScreen();//
    this->move((pScreen->geometry().width() - this->width()) / 2,(pScreen->geometry().height() - this->height()) / 2);
    _ui->toolBar->hide();
    _showWidget = new ShowWidget(_ui->widget);
    _ui->widget->layout()->addWidget(_showWidget);
    QWidget* widget2 = new QWidget(_ui->widget);
    _ui->widget->layout()->addWidget(widget2);
    widget2->hide();
    _canvasLabel = new CanvasLabel(_showWidget);
    _canvasLabel->hide();
    _statusBarLabel = new QLabel(this);
    _ui->statusbar->addWidget(_statusBarLabel);
    _initConnect();
    _decorationToolsBar();
    _openFile("/home/gao/preview.jpg");
}

MainWindow::~MainWindow() {
    delete _ui;
}

/**
 * change resize than re-render showwidget and canvaslabel
 * @param event
 */
void MainWindow::resizeEvent(QResizeEvent *event) {
    _ui->widget->resize(_ui->centralwidget->size());
    QMainWindow::resizeEvent(event);
}

/**
 * wheelEvent
 * @param event
 */
void MainWindow::wheelEvent(QWheelEvent *event) {
    if(_wheelEventFlag) {
        int n = event->angleDelta().y();
        // 处理滚轮加 Ctrl 键的事件
        if (event->modifiers() & Qt::ControlModifier) {
            _zoom(n);
        } else {
            if(n > 0) {
                _pulleyUP(n);
            }
            else {
                _pulleyDown(-n);
            }
        }
    }
}

/**
 * init connect
 * file acction clicked and open file widget
 * cancas label send message for status bar show
 */
void MainWindow::_initConnect() {
    connect(_ui->open,&QAction::triggered,this,&MainWindow::_openFileWidget);
    connect(_canvasLabel,&CanvasLabel::sendMessage,this,&MainWindow::setStatusBarMessage);
    connect(_showWidget,&ShowWidget::sizeChanged,_canvasLabel,&CanvasLabel::SizeChanged);
}

/**
 * decoration tools bar and init pushbottons connect
 */
void MainWindow::_decorationToolsBar() {
    QAction* button1 = _ui->toolBar->addAction("裁剪");
    connect(button1,&QAction::triggered,this,&MainWindow::_tailor);
}


/**
 * open file widget select a image file
 * transmit to canvas label to show image
 */
void MainWindow::_openFileWidget() {
    _filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "/home/gao", tr("Image Files (*.png *.jpg *.bmp *.jepg)"));
    _openFile(_filePath);
}

/**
 * transmit to canvas label to show image
 * @param path file path
 */
void MainWindow::_openFile(const QString &path) {
    _canvasLabel->show();
    if(_canvasLabel->ShowImage(path)) {
        _wheelEventFlag = true;
    }
}

/**
 * tailor image function
 */
void MainWindow::_tailor() {

}

void MainWindow::_zoom(int n) {
    //Zoom  from current center position
    _canvasLabel->Zoom(n/120);
}

void MainWindow::_pulleyUP(int) {

}


void MainWindow::_pulleyDown(int) {

}


/**
 * reset status bar label text
 * @param str message
 */
void MainWindow::setStatusBarMessage(QString str) {
    _statusBarLabel->setText(str);
}



