//
// Created by gao on 24-11-12.
//

#ifndef COVERLABEL_H
#define COVERLABEL_H

#include "clayer.h"

#include <deque>
//遮盖层
class CoverLayer :public CLayer{
    Q_OBJECT
public:
    explicit CoverLayer(QWidget *parent = nullptr);
    ~CoverLayer();
    void setRestrictedLayer(const CLayer*);//设置受限制层
    void effectBySizeOfLayer(const QSize&);//受其它层大小影响
    void effectByPosOfLayer(const QPoint&);//受其它层位置影响
    void setOffset(const QPoint &,int) override;//偏移
    void moveEnd();//结束移动
    bool moveBack();//修改返回
    void setDefaultLayer();//设置默认layout
    void showOption(QWidget*);
    QPoint getRelativePosition(){ return QPoint(_currentX,_currentY);}
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    int _x,_y;
    double _width,_height;
    int _currentX,_currentY;
    double _currentWidth,_currentHeight;
    int _partnerX,_partnerY;
    double _partnerWidth,_partnerHeight;
    int _direction = 3;
    std::deque<QRect> _memPosition;
    std::deque<QSize> _memPartnerSize;
    QRect _tmpPosition;
    QSize _tmpPartnerSize;
};



#endif //COVERLABEL_H
