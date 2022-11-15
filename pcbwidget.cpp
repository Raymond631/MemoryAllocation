#include "PCBwidget.h"
#include <QPainter>
#include <QStyleOption>

PCBwidget::PCBwidget(QWidget *parent)
    : QWidget{parent}
{

}

void PCBwidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    emit whichPCB(this);
}

void PCBwidget::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(e);
}
