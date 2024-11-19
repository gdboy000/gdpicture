//
// Created by gao on 24-11-11.
//

#ifndef SHOWWIDGET_H
#define SHOWWIDGET_H

#include <QLabel>


class CanvasLayer;
class CoverLayer;
class CLayer;

class ShowWidget : public QWidget {
Q_OBJECT

public:
    explicit ShowWidget(QWidget *parent = nullptr);
    ~ShowWidget() override;
    bool openFile(const QString &);//通过文件名打开文件

    void initCutMode();//初始化裁剪功能
    QRect getCutRange();//获取裁剪范围
    void saveCutRegion(const QString&);//保存裁剪区域

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    CanvasLayer* _canvasLayer = nullptr;//画布层
    CoverLayer* _coverLayer = nullptr;//遮盖层
    CLayer* _layer = nullptr;
    bool _leftMouseClicked = false;//鼠标左键点击状态
    bool _rightMouseClicked = false;//鼠标右键点击状态
    QPoint _startClickedPoint;//开始点击position
    QImage _showImage;//需要展示的图片
    QPoint _oldPos;//之前position
    int _coverLayerDirectionJudge(const QPoint&);//遮盖层鼠标位置判断和样式设置
    void _initConnect();//初始化连接

signals:
    void sizeChanges(QSize);
    void statusMessageChanges(const QString&);//消息改变信号

};


#endif //SHOWWIDGET_H
