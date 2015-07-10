/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medMeshVisibilityMenu.h>
#include <medLinkMenu.h>
#include <medListWidget.h>
#include <medSettingsManager.h>

class medMeshVisibilityMenuPrivate
{
public :
    QWidget *subPopupWidget;

    QStringList availableLabels;

    medListWidget *labelList;
    QList<double*> * listColors;

    QList<QPair<QString,int>> listAllAnatomicalLabels;
    QList<QPair<QString,int>> listAnatomicalLabelsForMesh;
};

medMeshVisibilityMenu::medMeshVisibilityMenu(QWidget * parent) : QPushButton(parent), d(new medMeshVisibilityMenuPrivate)
{
    d->subPopupWidget = new QWidget(this);
    d->subPopupWidget->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint );
    d->subPopupWidget->setContentsMargins(0,0,4,4);
    d->subPopupWidget->setObjectName("subPopupWidget");

    d->subPopupWidget->setAttribute(Qt::WA_TranslucentBackground);

#ifdef Q_OS_LINUX
    QGraphicsDropShadowEffect *shadowEffect1 = new QGraphicsDropShadowEffect(this);
    shadowEffect1->setOffset(2);
    shadowEffect1->setBlurRadius(8);
    shadowEffect1->setColor(QColor(33, 33, 33, 200));

    QGraphicsDropShadowEffect *shadowEffect2 = new QGraphicsDropShadowEffect(this);
    shadowEffect2->setOffset(2);
    shadowEffect2->setBlurRadius(8);
    shadowEffect2->setColor(QColor(33, 33, 33, 200));

    d->popupWidget->setGraphicsEffect(shadowEffect1);
    d->subPopupWidget->setGraphicsEffect(shadowEffect2);
#endif

    d->labelList = new medListWidget;
    d->labelList->setMouseTracking(true);
    d->labelList->setAlternatingRowColors(true);
    d->labelList->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);

    connect(this, SIGNAL(clicked()), this, SLOT(showPopup()));

    connect(d->labelList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(selectLabel(QListWidgetItem*)));
    connect(d->labelList, SIGNAL(itemEntered(QListWidgetItem*)), this, SLOT(highlightItem(QListWidgetItem*)));
    connect(d->labelList, SIGNAL(itemPressed(QListWidgetItem*)), this, SLOT(selectItem(QListWidgetItem*)));

    QWidget *internalSubPopWidget = new QWidget;
    internalSubPopWidget->setObjectName("internalSubPopWidget");
    QVBoxLayout *layout = new QVBoxLayout(d->subPopupWidget);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    layout->addWidget(internalSubPopWidget);

    QVBoxLayout *subPopUpLayout = new QVBoxLayout(internalSubPopWidget);
    subPopUpLayout->setContentsMargins(3,3,3,3);
    subPopUpLayout->setSpacing(3);
    subPopUpLayout->addWidget(d->labelList);

    d->labelList->installEventFilter(this);
    if(qApp->activeWindow())
      qApp->activeWindow()->installEventFilter(this);

    d->listAllAnatomicalLabels 
        << QPair<QString,int>("LVMyo",1) << QPair<QString,int>("LeftVentricle",5) 
        << QPair<QString,int>("LVMyo_LeftVentricle",170) << QPair<QString,int>("ApexLVEndo_LVMyo_LeftVentricle",15)
        << QPair<QString,int>("RightVentricle",90) << QPair<QString,int>("LVMyo_RightVentricle",25)
        << QPair<QString,int>("LeftAtrium",130) << QPair<QString,int>("LVMyo_LeftAtrium",35)
        << QPair<QString,int>("LeftAtrium_LeftVentricle_MitralValve",40) << QPair<QString,int>("RightAtrium",45)
        << QPair<QString,int>("RightAtrium_RightVentricle_TricuspidValve",50) << QPair<QString,int>("Aorta_AscendingAorta",55)
        << QPair<QString,int>("Aorta_AorticArch",60) << QPair<QString,int>("Aorta_DescendingAorta",65)
        << QPair<QString,int>("Aorta_AorticValve_LeftVentricle",70) << QPair<QString,int>("SupVenaCava",75)
        << QPair<QString,int>("RightAtrium_SupVenaCava_VirtualSupVenaCavaValve",80) << QPair<QString,int>("InfVenaCava",85)
        << QPair<QString,int>("InfVenaCava_RightAtrium_VirtualInfVenaCavaValve",20) << QPair<QString,int>("CoronarySinus",95)
        << QPair<QString,int>("PulmonaryArtery",100) << QPair<QString,int>("PulmonaryArtery_PulmonaryValve_RightVentricle",105)
        << QPair<QString,int>("SupRightPV",111) << QPair<QString,int>("LeftAtrium_SupRightPV_VirtualSupRightPVValve",115)
        << QPair<QString,int>("InfRightPV",120) << QPair<QString,int>("InfRightPV_LeftAtrium_VirtualInfRightPVValve",125)
        << QPair<QString,int>("SupLeftPV",30) << QPair<QString,int>("LeftAtrium_SupLeftPV_VirtualSupLeftPVValve",135)
        << QPair<QString,int>("InfLeftPV",140) << QPair<QString,int>("InfLeftPV_LeftAtrium_VirtualInfLeftPVValve",145)
        << QPair<QString,int>("VirtualAscendingAortaValve",150) << QPair<QString,int>("VirtualDescendingAortaValve",155)
        << QPair<QString,int>("CoronarySinus_RightAtrium_VirtualCoronarySinusValve",160) << QPair<QString,int>("LVMyoVolGridScar",110)
        << QPair<QString,int>("LVMyoVolGrid",31) << QPair<QString,int>("LeftAtriumPulmonaryVeinCap",2)
        << QPair<QString,int>("Aorta",10) << QPair<QString,int>("None_Subpart",165); 
}

medMeshVisibilityMenu::~medMeshVisibilityMenu()
{
    delete d;
}

void medMeshVisibilityMenu::setAvailableLabels(QStringList labels)
{
    d->availableLabels = labels;

    d->labelList->clear();

    QListWidgetItem * item = new QListWidgetItem("All");
    //item->setSizeHint(QSize(item->sizeHint().width(), 20));
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Checked);
    d->labelList->insertItem(0,item);

    int i = 1;
    foreach(QString label, labels)
    {
        double r = (double)(d->listColors->at(i-1)[0])*255;
        double g = (double)(d->listColors->at(i-1)[1])*255;
        double b = (double)(d->listColors->at(i-1)[2])*255;
        QPixmap pix(15,15);
        pix.fill(QColor(r,g,b,255));
        QListWidgetItem * item = new QListWidgetItem(d->listAnatomicalLabelsForMesh[i-1].first);
        //item->setSizeHint(QSize(50, 20));
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Checked);
        item->setIcon(QIcon(pix));
        d->labelList->insertItem(i,item);
        i++;
    }
    d->labelList->setMaximumHeight(300);
    d->labelList->setMinimumWidth(300);
    d->subPopupWidget->setMinimumWidth(300);
    d->subPopupWidget->setMaximumHeight(300);
}

void medMeshVisibilityMenu::selectLabel(QListWidgetItem *item)
{
    /*QString group = d->groupList->currentItem()->data(Qt::UserRole).toString();
    bool groupChecked = d->groupList->currentItem()->checkState() == Qt::Checked;*/
    QString label = item->text();

    if(label == "All")
    {
        if(item->checkState() == Qt::Checked)
          checkAllLabels(true);
        else checkAllLabels(false);
        return;
    }
    int i = d->labelList->row(item);
    if(item->checkState() == Qt::Checked)
    {
        emit labelChecked(d->listAnatomicalLabelsForMesh[i-1].second);
    }
    else
    {
        emit labelUnchecked(d->listAnatomicalLabelsForMesh[i-1].second);

        // a param has been unchecked, uncheck 'All' item
        d->labelList->blockSignals(true);
        d->labelList->item(0)->setCheckState(Qt::Unchecked);
        d->labelList->blockSignals(false);
    }
}

void medMeshVisibilityMenu::showPopup()
{
    if(!d->subPopupWidget->isVisible())
    {
        QPoint globalPos = mapToGlobal(QPoint(0,0));

        d->subPopupWidget->move( globalPos.x(), globalPos.y() + this->height());

        d->subPopupWidget->resize(d->subPopupWidget->width(), d->labelList->sizeHint().height());
        d->subPopupWidget->show();
    }
    else
    {
        d->subPopupWidget->hide();
    }
}

void medMeshVisibilityMenu::hideMenus()
{
    d->subPopupWidget->hide();
}

void medMeshVisibilityMenu::hideSubMenu()
{
    d->subPopupWidget->hide();
}

void medMeshVisibilityMenu::paintEvent(QPaintEvent *ev)
{
    QPushButton::paintEvent(ev);

    updateListsPosition();
}

bool medMeshVisibilityMenu::eventFilter(QObject *object, QEvent *event)
{
    if(event->type() == QEvent::FocusOut)
    {
        QPoint cursor = QCursor::pos();

        QPoint p1 = d->subPopupWidget->mapToGlobal(QPoint(0,0));
        QRect rect1( p1, d->subPopupWidget->size() );

        QPoint p2 = this->mapToGlobal(QPoint(0,0));
        QRect rect2( p2, this->size() );

        if(!rect1.contains(cursor) && !rect2.contains(cursor))
        {
            hideMenus();
        }
    }
    else if(object == qApp->activeWindow() && event->type() == QEvent::Move)
    {
        hideMenus();
    }
    else if(object == d->labelList && event->type() == QEvent::Leave)
    {
        d->labelList->clearSelection();
    }

    return false;

}

void medMeshVisibilityMenu::updateListsPosition()
{
    QPoint globalPos = mapToGlobal(QPoint(0,0));
    d->subPopupWidget->move( globalPos.x(), globalPos.y() + this->height());
}

void medMeshVisibilityMenu::highlightItem(QListWidgetItem *item)
{
    item->setSelected(true);
}

void medMeshVisibilityMenu::checkAllLabels(bool check)
{
    for(int i=0; i<d->labelList->count(); i++)
    {
        QListWidgetItem *item = d->labelList->item(i);
        if(check)
        {
            item->setCheckState(Qt::Checked);
        }
        else
        {
            item->setCheckState(Qt::Unchecked);
        }
    }
}

void medMeshVisibilityMenu::selectItem(QListWidgetItem *item)
{
    if(item->checkState() == Qt::Checked)
    {
        item->setCheckState(Qt::Unchecked);
    }
    else
    {
        item->setCheckState(Qt::Checked);
    }
}

void medMeshVisibilityMenu::setLabels(QList<double*> * listValues)
{
    d->availableLabels.clear();
    for(int i=0;i<listValues->count();i++)
    {
        d->availableLabels << QString::number(i);
        double r = (double)(listValues->at(i)[0])*255;
        double g = (double)(listValues->at(i)[1])*255;
        double b = (double)(listValues->at(i)[2])*255;
    }
    d->listColors = listValues;
    this->setAvailableLabels(d->availableLabels);
}

void medMeshVisibilityMenu::setAnatomicalLabelsForMesh(QList<QPair<QString,int> > list )
{
    d->listAnatomicalLabelsForMesh = list;
}

QList<QPair<QString,int> > medMeshVisibilityMenu::getAllAnatomicalLabels()
{
    return d->listAllAnatomicalLabels;
}

QList<QPair<QString,int> > medMeshVisibilityMenu::getAnatomicalLabelsForMesh()
{
    return d->listAnatomicalLabelsForMesh;
}
