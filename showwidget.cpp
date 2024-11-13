//
// Created by gao on 24-11-11.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ShowWidget.h" resolved

#include "showwidget.h"

#include <oneapi/tbb/detail/_template_helpers.h>


ShowWidget::ShowWidget(QWidget *parent) :
    QWidget(parent) {

}

ShowWidget::~ShowWidget() {

}

// void ShowWidget::showEvent(QShowEvent *event) {
//     qDebug()<<"show size: "<<this->width()<<"  "<<this->height();
//     emit sizeChanged(this->size());
//     QWidget::showEvent(event);
// }

void ShowWidget::resizeEvent(QResizeEvent *event) {
    emit sizeChanged(this->size());
    QWidget::resizeEvent(event);

}
