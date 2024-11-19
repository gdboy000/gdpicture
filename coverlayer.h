//
// Created by gao on 24-11-12.
//

#ifndef COVERLABEL_H
#define COVERLABEL_H

#include "clayer.h"

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
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    int _x,_y;
    double _width,_height;
    int _currentX,_currentY;//该position是相较于受限制层的位置
    double _currentWidth,_currentHeight;
    int _partnerX,_partnerY;
    double _partnerWidth,_partnerHeight;
};



#endif //COVERLABEL_H
