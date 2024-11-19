//
// Created by gao on 24-11-15.
//

#ifndef CLAYER_H
#define CLAYER_H

#include <QLabel>

class CLayer:public QLabel {
    Q_OBJECT
public:
    CLayer(QWidget* parent):QLabel(parent){}
    virtual ~CLayer(){}
    virtual void setSize(const QSize& size) {
        this->resize(size);
    }
    virtual void setPosition(const QPoint& point) {
        this->move(point);
    }
    virtual void setOffset(const QPoint& point){
        this->move(this->pos()+point);
    }
    virtual void setOffset(const QPoint& point,int n=0) {
        this->move(this->pos()+point);
    }
signals:
    void moved(const QPoint&);
    void resized(const QSize&);

};

#endif //CLAYER_H
