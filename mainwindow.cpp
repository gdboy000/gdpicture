//
// Created by gao on 24-11-11.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved
#include <QFileDialog>
#include <QPushButton>
#include <QWheelEvent>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "showwidget.h"
#include "savefiledialog.h"

#define SHOW(str) qDebug()<<str

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), _ui(new Ui::MainWindow) {
    _ui->setupUi(this);

    const QScreen *pScreen = QGuiApplication::primaryScreen();//
    this->move((pScreen->geometry().width() - this->width()) / 2,(pScreen->geometry().height() - this->height()) / 2);
    _ui->menubar->setContextMenuPolicy(Qt::PreventContextMenu);

    _ui->toolBar->hide();//默认隐藏工具栏
    //展示窗口
    _showWidget = new ShowWidget(_ui->widget);
    _ui->widget->layout()->addWidget(_showWidget);

    _saveDialog = new SaveFileDialog(this);
    //工具窗口，需要设置最大宽度
    _toolWidget = new QWidget(_ui->widget);
    _ui->widget->layout()->addWidget(_toolWidget);
    _toolWidget->hide();
    //消息栏设置两个label
    _statusBarLabel1 = new QLabel(this);
    _statusBarLabel1->setFixedSize(250,20);
    _ui->statusbar->addWidget(_statusBarLabel1);

    _statusBarLabel2 = new QLabel(this);
    _ui->statusbar->addWidget(_statusBarLabel2);

    _initConnect();
    _decorationToolsBar();
}

MainWindow::~MainWindow() {
    delete _ui;
}

/**
 * change resize than re-render showwidget and canvaslabel
 * @param event
 */
void MainWindow::resizeEvent(QResizeEvent *event) {
    _ui->widget->resize(_ui->centralwidget->size());//未设置布局，不会主动填充
    QMainWindow::resizeEvent(event);
}

/**
 * wheelEvent
 * @param event
 */
void MainWindow::wheelEvent(QWheelEvent *event) {
}

/**
 * init connect
 * file acction clicked and open file widget
 * cancas label send message for status bar show
 */
void MainWindow::_initConnect() {
    connect(_ui->open,&QAction::triggered,this,&MainWindow::_openFileWidget);
    connect(_ui->save,&QAction::triggered,this,&MainWindow::_saveImageSelect);
    connect(_ui->close,&QAction::triggered,[this]{this->close();});
    connect(_ui->tool,&QAction::triggered,this,&MainWindow::_toolsBarShow);
    connect(_showWidget,&ShowWidget::statusMessageChanges,[this](const QString& str){_statusBarLabel2->setText(str);});
    connect(_saveDialog,&SaveFileDialog::savePathChanged,this,&MainWindow::_saveImage);
}

/**
 * decoration tools bar and init pushbottons connect
 */
void MainWindow::_decorationToolsBar() {
    QFont font("Times", 15, QFont::Bold);
    _ui->toolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    _ui->toolBar->setStyleSheet("QToolBar {padding:0px;background-color:rgb(200,200,200);}QToolBar QToolButton{font: bold 17px Arial;background-color: transparent;border:none;}QToolBar QToolButton:hover{background-color: rgba(255, 255, 255,0.7);}");
    _ui->toolBar->setFont(font);
    QWidget* widget = new QWidget(this);
    widget->setMinimumHeight(30);
    _ui->toolBar->addWidget(widget);
    QAction *button1 = _ui->toolBar->addAction("裁剪");

    button1->setToolTip("裁剪");
    connect(button1,&QAction::triggered,_showWidget,&ShowWidget::initCutMode);//
}


/**
 * open file widget select a image file
 * transmit to canvas label to show image
 */
void MainWindow::_openFileWidget() {
    _filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "/home/gao", tr("Image Files (*.png *.jpg *.bmp *.jepg)"));
    if(!_filePath.isEmpty() && _showWidget->openFile(_filePath)){
        _wheelEventFlag = true;
        _toolsBarShow(true);
        _statusBarLabel1->setText(_filePath);
        QFontMetrics fm(_statusBarLabel1->font());
        _statusBarLabel1->setText(fm.elidedText(_filePath, Qt::ElideMiddle, _statusBarLabel1->width()));
    }
    else {
        _statusBarLabel1->setText(QString("%1 文件打开失败").arg(_filePath));
    }
}

/**
 * tools bar to display,if show is true force show
 */
void MainWindow::_toolsBarShow(bool show) const{
    if(!_wheelEventFlag) {_statusBarLabel1->setText("无操作图像"); return;}
    if(this->_ui->toolBar->isVisible() && !show) this->_ui->toolBar->hide();
    else this->_ui->toolBar->show();
    _ui->widget->resize(_ui->centralwidget->size());
}
void MainWindow::_showOptionWidget() {
    _optionWidget = new QWidget(this);
    _optionWidget->setStyleSheet("QWidget {border:1px solid rgba(0,0,0,0.5);border-radius:3px;} QWidget QPushButton{min-width:30px;min-height:33px;border:none;font-size:10px;} QWidget QPushButton:hover{border-radius:0px;}");
    QPushButton* confirmBtn = new QPushButton(_optionWidget);
    confirmBtn->setStyleSheet(":hover{background-color:green;}");
    confirmBtn->setText("确认");
    QPushButton* closeBtn = new QPushButton(_optionWidget);
    closeBtn->setText("关闭");
    closeBtn->setStyleSheet(":hover{background-color:red}");
    QPushButton* flushBtn = new QPushButton(_optionWidget);
    flushBtn->setStyleSheet(":hover{background-color:black;color:white;}");
    flushBtn->setText("刷新");
    _optionWidget->setLayout(new QHBoxLayout);
    _optionWidget->layout()->setContentsMargins(1,0,1,0);
    _optionWidget->layout()->addWidget(confirmBtn);
    _optionWidget->layout()->addWidget(closeBtn);
    _optionWidget->layout()->addWidget(flushBtn);
    _optionWidget->setFixedSize(100,35);
    _optionWidget->layout()->setSpacing(0);
    _optionWidget->move(100,100);
    _optionWidget->hide();
    connect(confirmBtn,&QPushButton::clicked,[this]{});
    connect(closeBtn,&QPushButton::clicked,[this]{});
    connect(flushBtn,&QPushButton::clicked,[this]{});
}

/**
 * 菜单栏另存为点击进入此函数
 * 打开文件保存配置窗口
 */
void MainWindow::_saveImageSelect() {
    if(!_wheelEventFlag) {
        _statusBarLabel1->setText("无操作图像");
        return;
    }
    QFileInfo fileInfo(_filePath);
    _saveDialog->setDefaultPath(fileInfo.absoluteDir().path(),fileInfo.completeBaseName()+"_1",fileInfo.suffix());
    _saveDialog->open();
}

/**
 * 文件保存窗口点击确认后
 * 失败不关闭文件保存窗口并显示问题
 * @param path 保存路径
 */
void MainWindow::_saveImage(const QString &path) {
    QString result;
    if(_showWidget->saveCutRegion(path,result)) {
        _statusBarLabel1->setText("保存成功");
        _saveDialog->close();
    }
    else {
        qDebug()<<result;
        _saveDialog->setResultMessage(result);
    }
}





