#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QTimer>
#include <QKeyEvent>
#include "PCBwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct mypcb
{
    int id;
    QString name;//PID
    int time;
    int vip;
    QString status;
    int start;//起始地址
    int needMemory;//需要内存
    mypcb(int i,QString n,int t,int v,QString s,int st,int need)
    {
        id=i;
        name=n;
        time=t;
        vip=v;
        status=s;
        start=st;
        needMemory=need;
    }
    mypcb()
    {

    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void keyPressEvent(QKeyEvent *event) override;    //设置回车响应函数

private slots:
    void refresh();
    void on_btnAddProcess_clicked();

    void on_btnWaiting_clicked();
    void on_btnDeWaiting_clicked();

    void Suspend(PCBwidget *p);
    void on_btnDeSuspend_clicked();

private:
    Ui::MainWindow *ui;

    int pcbID=1;
    QList<mypcb> pcb_list;
    QList<mypcb> pcb_ready;
    QList<mypcb> pcb_waiting;
    QList<mypcb> pcb_suspend;
    mypcb pcb_running;

    int timeSlice;//时间片大小
    int counter=0;

    int memory[777];//内存条，总共777个单位大小

    QTimer *timer;//计时器

    bool checkName(QString name);
    void sortPCB(QList<mypcb> &list,int length);
    void removePCB(QList<mypcb> &list,mypcb aPcb);

    void addToList(QList<mypcb> list,QListWidget* listWidget);
    void addPCB(mypcb aPcb,QListWidget* listWidget);

    int selectBlock();
    void addMemoryList(QString pid,int start,int needMemory);

    void refreshStatus(QString name,QString status);
};
#endif // MAINWINDOW_H
