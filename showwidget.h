//
// Created by gao on 24-11-11.
//

#ifndef SHOWWIDGET_H
#define SHOWWIDGET_H

#include <QWidget>



class ShowWidget : public QWidget {
Q_OBJECT

public:
    explicit ShowWidget(QWidget *parent = nullptr);
    ~ShowWidget() override;
protected:
    //void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
signals:
    void sizeChanged(QSize);

};


#endif //SHOWWIDGET_H
