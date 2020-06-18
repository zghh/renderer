#pragma once
#include <QWidget>
#include <QLabel>

#ifndef PICTURELABEL_H
#define PICTURELABEL_H

class PictureLabel : public QLabel
{
    Q_OBJECT
public:
    explicit PictureLabel(QWidget *parent = nullptr);

protected:
    void mouseReleaseEvent(QMouseEvent *event);     //抬起
    void mousePressEvent(QMouseEvent *event);       //按下
    void mouseDoubleClickEvent(QMouseEvent *event); //双击
    void mouseMoveEvent(QMouseEvent *event);        //拖动
    void wheelEvent(QWheelEvent *event);         //滚轮
signals:
    void clicked(QMouseEvent *event);  //抬起
    void pressed(QMouseEvent *event);  //按下
    void doubled(QMouseEvent *event);  //双击
    void moved(QMouseEvent *event);    //拖动
    void wheel(QWheelEvent *event); //滚轮
public slots:
};

#endif // PICTURELABEL_H
