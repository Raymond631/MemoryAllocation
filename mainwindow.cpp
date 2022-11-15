#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QTimer>
#include <QCheckBox>
#include <QObject>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timer = new QTimer(this);//计时器
    QObject::connect(timer,SIGNAL(timeout()),this,SLOT(refresh()));
    timer->start(1000);//1秒刷新一次

    ui->pcb_name->setFocus();
    ui->tabWidget->setCurrentIndex(0);

    //分配操作系统内存
    QLabel* n=new QLabel("操作系统");
    QLabel* y=new QLabel("起址：0");
    QLabel* l=new QLabel("长度：100");

    QGridLayout *layout=new QGridLayout();
    layout->addWidget(n,0,0,1,2);
    layout->addWidget(y,1,0,1,1);
    layout->addWidget(l,1,1,1,1);

    QWidget* w=new QWidget(this);
    w->setLayout(layout);
    w->setStyleSheet("QWidget{font:12pt;font-family:Times New Roman;background-color:red}");
    w->setGeometry(QRect(414,103,388,100));//x,y,width,heigth;原点坐标414，103，宽度388，总高度777

    //初始化内存条
    for(int i=0;i<100;i++)//操作系统id为0
    {
        memory[i]=0;
    }
    for(int i=100;i<777;i++)//未分配分区id为-1
    {
        memory[i]=-1;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
    {
        on_btnAddProcess_clicked();
        ui->pcb_name->setFocus();
    }
}
bool MainWindow::checkName(QString name)
{
    for(mypcb p : pcb_list)
    {
        if(p.name==name)
        {
            return false;
        }
    }
    return true;
}
void MainWindow::sortPCB(QList<mypcb> &list, int length)
{
    for(int i=0;i<length-1;i++)
    {
        for(int j=0;j<length-i-1;j++)
        {
            if(list.at(j).vip<list.at(j+1).vip)
            {
                continue;
            }
            else if(list.at(j).vip>list.at(j+1).vip)
            {
                mypcb temp=list.at(j);
                list.replace(j,list.at(j+1));
                list.replace(j+1,temp);
            }
            else
            {
                if(list.at(j).id<list.at(j+1).id)
                {
                    continue;
                }
                else if(list.at(j).id>list.at(j+1).id)
                {
                    mypcb temp=list.at(j);
                    list.replace(j,list.at(j+1));
                    list.replace(j+1,temp);
                }
                else
                {
                    QMessageBox::warning(NULL,"","ERROR2!");
                }
            }
        }
    }
}
void MainWindow::removePCB(QList<mypcb> &list, mypcb aPcb)
{
    int i=0;
    for(mypcb p:list)
    {
        if(p.id==aPcb.id)
        {
            list.removeAt(i);
        }
        i++;
    }
}

void MainWindow::on_btnAddProcess_clicked()
{

    QString name=ui->pcb_name->text();
    int time=ui->pcb_time->text().toInt();
    int vip=ui->pcb_vip->text().toInt();
    int neddMemory=ui->pcb_memory->text().toInt();

    if(name==NULL)
    {
        QMessageBox::warning(NULL,"","进程信息不能为空!");
        ui->pcb_name->setFocus();
    }
    else
    {
        if(checkName(name))//进程名查重
        {
            int start =selectBlock();
            if(start<0)//内存块查询
            {
                QMessageBox::warning(NULL,"","未找到合适空白区!");
            }
            else
            {
                addMemoryList(name,start,neddMemory);//画内存条内信息

                mypcb aPCB(pcbID,name,time,vip,"ready",start,neddMemory);
                pcbID++;

                pcb_list.append(aPCB);
                pcb_ready.append(aPCB);

                sortPCB(pcb_ready,pcb_ready.length());//优先级排序
                addToList(pcb_ready,ui->ReadyPCB);

                ui->pcb_name->clear();
                ui->pcb_time->clear();
                ui->pcb_vip->clear();
                ui->pcb_memory->clear();
            }
        }
        else
        {
            QMessageBox::warning(NULL,"","进程名已存在!");
            ui->pcb_name->clear();
        }
    }
}
int MainWindow::selectBlock()
{
    int needMemory=ui->pcb_memory->text().toInt();
    int length=0;
    bool status=false;//是否在计算长度
    int start=-1;//起始地址
    for(int i=100;i<777;i++)
    {
        if(status)
        {
            if(memory[i]<0)//继续计数
            {
                length++;
            }
            else//计完一个空闲区
            {
                status=false;
                //判断长度是否满足要求
                if(length>=needMemory)
                {
                    return start;
                }
                else
                {
                    length=0;
                    start=-1;
                }
            }
        }
        else
        {
            if(memory[i]<0)//开始计数
            {
                status=true;
                start=i;//记录起始地址
                length++;
            }
        }
    }
    if(length>=needMemory)
    {
        return start;
    }
    else//当最后一个单位为空，但总长度不够时
    {
        return -1;
    }
}
void MainWindow::addMemoryList(QString pid,int start,int needMemory)
{
    QLabel* n=new QLabel(pid);
    n->setObjectName(QStringLiteral("name"));//方便勾选时 查找选了哪个pcb
    QLabel* y=new QLabel("起址："+QString::number(start));
    QLabel* l=new QLabel("长度："+QString::number(needMemory));

    QGridLayout *layout=new QGridLayout();
    layout->addWidget(n,0,0,1,1);
    layout->addWidget(y,1,0,1,1);
    layout->addWidget(l,1,1,1,1);

    PCBwidget* w=new PCBwidget(this);
    w->setObjectName(pid);//方便删除
    w->setLayout(layout);

    //随机颜色
    QColor clr(rand() % 256, rand() % 256, rand() % 256);
    QString cs=QString("*{font:12pt;font-family:Times New Roman;background-color:%1}").arg(clr.name());
    w->setStyleSheet(cs);

    w->setGeometry(QRect(414,103+start,388,needMemory));//x,y,width,heigth;原点坐标414，103，宽度388，总高度777
    w->show();

    QObject::connect(w,SIGNAL(whichPCB(PCBwidget*)),this,SLOT(Suspend(PCBwidget*)));

    for(int i=start;i<start+needMemory;i++)
    {
        memory[i]=1;
    }
}

void MainWindow::refresh()
{
    if(ui->RunningPCB->count()==0&&ui->ReadyPCB->count()!=0)
    {
        //添加进running
        pcb_running=pcb_ready.at(0);
        refreshStatus(pcb_running.name,"running");
        addPCB(pcb_running,ui->RunningPCB);

        //从ready移除
        pcb_ready.removeFirst();
        addToList(pcb_ready,ui->ReadyPCB);
    }
    else if(ui->RunningPCB->count()==1)
    {
        if(pcb_running.time==0)
        {
            //从running移除
            ui->RunningPCB->clear();
            //从PCBlist移除

            QWidget* w=this->findChild<QWidget*>(pcb_running.name);
            w->close();
            for(int i=pcb_running.start;i<=pcb_running.start+pcb_running.needMemory;i++)
            {
                memory[i]=-1;
            }

            removePCB(pcb_list,pcb_running);
        }
        else
        {
            pcb_running.time--;
            //刷新running序列
            ui->RunningPCB->clear();
            addPCB(pcb_running,ui->RunningPCB);

            for(mypcb &m:pcb_list)
            {
                if(m.name==pcb_running.name)
                {
                    m.time=pcb_running.time;
                }
            }
        }
    }
}
void MainWindow::refreshStatus(QString name, QString status)
{
    for(mypcb &p : pcb_list)
    {
        if(p.name==name)
        {
            p.status=status;
        }
    }

    if(status=="running")
    {
        pcb_running.status=status;
    }
    else if(status=="ready")
    {
        for(mypcb &p : pcb_ready)
        {
            if(p.name==name)
            {
                p.status=status;
            }
        }
    }
    else if(status=="waiting")
    {
        for(mypcb &p : pcb_waiting)
        {
            if(p.name==name)
            {
                p.status=status;
            }
        }
    }
    else if(status=="suspend")
    {
        for(mypcb &p : pcb_suspend)
        {
            if(p.name==name)
            {
                p.status=status;
            }
        }
    }
}

void MainWindow::addToList(QList<mypcb> list, QListWidget *listWidget)
{
    listWidget->clear();
    for(mypcb aPcb:list)
    {
        addPCB(aPcb,listWidget);
    }
}
void MainWindow::addPCB(mypcb aPcb, QListWidget *listWidget)
{
    QLabel* n=new QLabel("PID："+aPcb.name);
    n->setObjectName(QStringLiteral("name"));//方便勾选时 查找选了哪个pcb
    QLabel* t=new QLabel("time："+QString::number(aPcb.time));
    QLabel* v=new QLabel("priority："+QString::number(aPcb.vip));
    QLabel* s=new QLabel("status："+aPcb.status);
    QCheckBox* c=new QCheckBox();
    c->setObjectName(QStringLiteral("checkbox"));


    QGridLayout *layout=new QGridLayout();
    layout->addWidget(n,0,0,1,1);
    layout->addWidget(t,0,1,1,1);
    layout->addWidget(v,1,0,1,1);
    layout->addWidget(s,1,1,1,1);
    layout->addWidget(c,0,2,2,1,Qt::AlignCenter);

    QWidget* w=new QWidget();
    w->setLayout(layout);
    w->setStyleSheet("QWidget{font:12pt;font-family:Times New Roman}");

    QListWidgetItem *item=new QListWidgetItem();
    listWidget->addItem(item);
    listWidget->setItemWidget(item,w);
}

void MainWindow::on_btnWaiting_clicked()
{
    timer->stop();

    ui->RunningPCB->clear();
    pcb_waiting.append(pcb_running);
    refreshStatus(pcb_running.name,"waiting");
    addToList(pcb_waiting,ui->WaitingPCB);

    timer->start(1000);
}
void MainWindow::on_btnDeWaiting_clicked()
{
    timer->stop();

    for(int i=0;i<ui->WaitingPCB->count();i++)
    {
        QListWidgetItem* item=ui->WaitingPCB->item(i);
        QCheckBox* box=ui->WaitingPCB->itemWidget(item)->findChild<QCheckBox*>("checkbox");
        if(box->isChecked())
        {
            QString name=ui->WaitingPCB->itemWidget(item)->findChild<QLabel*>("name")->text().mid(4);
            for(mypcb m:pcb_waiting)
            {
                if(m.name==name)
                {
                    removePCB(pcb_waiting,m);
                    pcb_ready.append(m);
                    refreshStatus(name,"ready");
                }
            }
        }
    }
    sortPCB(pcb_ready,pcb_ready.length());

    addToList(pcb_waiting,ui->WaitingPCB);
    addToList(pcb_ready,ui->ReadyPCB);

    timer->start(1000);
}

void MainWindow::Suspend(PCBwidget* p)
{
    timer->stop();

    QString name=p->findChild<QLabel*>("name")->text();
    for(mypcb m:pcb_list)
    {
        if(m.name==name)
        {
            QString status=m.status;
            if(status=="suspend")
            {
                continue;
            }
            else if(status=="running")
            {
                ui->RunningPCB->clear();
            }
            else if(status=="ready")
            {
                removePCB(pcb_ready,m);
                addToList(pcb_ready,ui->ReadyPCB);
            }
            else if(status=="waiting")
            {
                removePCB(pcb_waiting,m);
                addToList(pcb_waiting,ui->WaitingPCB);
            }
            pcb_suspend.append(m);
        }
    }

    for(mypcb p:pcb_suspend)
    {
        refreshStatus(p.name,"suspend");
    }
    addToList(pcb_suspend,ui->SuspendPCB);

    timer->start(1000);
}
void MainWindow::on_btnDeSuspend_clicked()
{
    timer->stop();

    for(int i=0;i<ui->SuspendPCB->count();i++)
    {
        QListWidgetItem* item=ui->SuspendPCB->item(i);
        QCheckBox* box=ui->SuspendPCB->itemWidget(item)->findChild<QCheckBox*>("checkbox");
        if(box->isChecked())
        {
            QString name=ui->SuspendPCB->itemWidget(item)->findChild<QLabel*>("name")->text().mid(4);
            for(mypcb m:pcb_list)
            {
                if(m.name==name)
                {
                    removePCB(pcb_suspend,m);
                    pcb_ready.append(m);
                    refreshStatus(name,"ready");
                }
            }
        }
    }
    sortPCB(pcb_ready,pcb_ready.length());

    addToList(pcb_suspend,ui->SuspendPCB);
    addToList(pcb_ready,ui->ReadyPCB);

    timer->start(1000);
}
