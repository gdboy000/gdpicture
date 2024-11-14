# 图片编辑器

## 2024/11/13首次提交

**当前已完成图片缩放，拖拉。裁剪掩膜遮盖，改变大小**

**问题：**

1. 图片缩放时需要参考窗口的比例，当前存在部分图片在改变窗口尺寸时，图片大小改变。
   1. 可能是长宽没有同一倍率增长。
2. 掩膜层在窗口尺寸改变时，不会按比例改变大小。
   1. 掩膜层没有铺满情况下，放大，缩小的x,y未改变。
   2. 主要解决在收到resize和move时的操作，且如何区分第一次直接赋值操作。

## 2024/11/13提交

**图像缩放推拉功能改变**

**详细描述：**

1. 改变之前在移动或改变图像大小后，其位置相对于窗口大小改变也会移动。现在改成无论图像是否移动或改变大小，在窗口改变时，恢复默认位置，大小。因为考虑到实际应用情况下，不需要这么复杂。
2. 解决掉图像放大还原后，图像位置出现偏移问题。中心放大缩小偏移好解决，但是移动后再放大缩小偏移未解决。于是在移动后，放大缩小的图像x,y坐标不变。