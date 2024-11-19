//
// Created by gao on 24-11-11.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLabel;
class ShowWidget;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
protected:
    //void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
private:
    Ui::MainWindow* _ui;
    QString _filePath;
    ShowWidget* _showWidget = nullptr;
    QWidget* _toolWidget = nullptr;
    QLabel* _statusBarLabel1 = nullptr;
    QLabel* _statusBarLabel2 = nullptr;
    bool _wheelEventFlag = false;
    void _initConnect();//init connect
    void _openFileWidget();//open file widget
    void _decorationToolsBar();
    void _toolsBarShow() const;
};


#endif //MAINWINDOW_H
