#include "pictureLabel.h"

PictureLabel::PictureLabel(QWidget *parent) : QLabel(parent)
{
}

void PictureLabel::mouseReleaseEvent(QMouseEvent *event)
{
    emit clicked(event);
}

void PictureLabel::mousePressEvent(QMouseEvent *event)
{
    emit pressed(event);
}

void PictureLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit doubled(event);
}

void PictureLabel::mouseMoveEvent(QMouseEvent *event)
{
    emit moved(event);
}

void PictureLabel::wheelEvent(QWheelEvent *event)
{
    emit wheel(event);
}