#ifndef PCBWIDGET_H
#define PCBWIDGET_H

#include <QWidget>
#include <QDebug>

class PCBwidget : public QWidget
{
    Q_OBJECT
public:
    explicit PCBwidget(QWidget *parent = nullptr);

signals:
    void whichPCB(PCBwidget* p);

protected:
    void mouseDoubleClickEvent(QMouseEvent* e) override;
    void paintEvent(QPaintEvent* e) override;


};

#endif // PCBWIDGET_H
