/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medMeshVisibilityMenu.h>
#include <medBoolParameter.h>
#include <medLinkMenu.h>
#include <medListWidget.h>
#include <medSettingsManager.h>

class medMeshVisibilityMenuPrivate
{
public :
    QWidget *subPopupWidget;

    QStringList availableLabels;

    medLabelsVisibilityParameter * labelsParameter;

    //medListWidget *labelList;
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

    connect(this, SIGNAL(clicked()), this, SLOT(showPopup()));
    
    d->labelsParameter = new medLabelsVisibilityParameter("MeshLabelsVisibility");
    connect(d->labelsParameter, SIGNAL(labelChecked(QString)), this, SLOT(emitLabelChecked(QString)));
    connect(d->labelsParameter, SIGNAL(labelUnchecked(QString)), this, SLOT(emitLabelUnchecked(QString)));

    QWidget *internalSubPopWidget = new QWidget;
    internalSubPopWidget->setObjectName("internalSubPopWidget");
    QVBoxLayout *layout = new QVBoxLayout(d->subPopupWidget);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    layout->addWidget(internalSubPopWidget);

    QVBoxLayout *subPopUpLayout = new QVBoxLayout(internalSubPopWidget);
    subPopUpLayout->setContentsMargins(3,3,3,3);
    subPopUpLayout->setSpacing(3);
    subPopUpLayout->addWidget(d->labelsParameter->getWidget());

    d->labelsParameter->getWidget()->installEventFilter(this);
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
    d->labelsParameter->clearWidget();
    d->labelsParameter->addLabel("All",true);
    
    int i = 1;
    foreach(QString label, labels)
    {
        double r = (double)(d->listColors->at(i-1)[0])*255;
        double g = (double)(d->listColors->at(i-1)[1])*255;
        double b = (double)(d->listColors->at(i-1)[2])*255;
        d->labelsParameter->addLabel(d->listAnatomicalLabelsForMesh[i-1].first,true,QColor(r,g,b,255));
        i++;
    }
    
    d->subPopupWidget->setMinimumWidth(300);
    d->subPopupWidget->setMaximumHeight(300);
}


void medMeshVisibilityMenu::showPopup()
{
    if(!d->subPopupWidget->isVisible())
    {
        QPoint globalPos = mapToGlobal(QPoint(0,0));

        d->subPopupWidget->move( globalPos.x(), globalPos.y() + this->height());

        d->subPopupWidget->resize(d->subPopupWidget->width(), d->labelsParameter->getWidget()->sizeHint().height());
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
    else if(object == d->labelsParameter->getWidget() && event->type() == QEvent::Leave)
    {
        static_cast<medListWidget*>(d->labelsParameter->getWidget())->clearSelection();
    }

    return false;
}

void medMeshVisibilityMenu::updateListsPosition()
{
    QPoint globalPos = mapToGlobal(QPoint(0,0));
    d->subPopupWidget->move( globalPos.x(), globalPos.y() + this->height());
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

void medMeshVisibilityMenu::emitLabelChecked(QString label)
{
    for(int i=0;i<d->listAnatomicalLabelsForMesh.count();i++)
    {
        if (d->listAnatomicalLabelsForMesh[i].first==label)
        {
            emit labelChecked(d->listAnatomicalLabelsForMesh[i].second);
            return;
        }
    }
}

void medMeshVisibilityMenu::emitLabelUnchecked(QString label)
{
    for(int i=0;i<d->listAnatomicalLabelsForMesh.count();i++)
    {
        if (d->listAnatomicalLabelsForMesh[i].first==label)
        {
            emit labelUnchecked(d->listAnatomicalLabelsForMesh[i].second);
            return;
        }
    }
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

medLabelsVisibilityParameter * medMeshVisibilityMenu::getLabelsParameter()
{
    return d->labelsParameter;
}

///////////////////////////////////medLabelsVisibilityParameter//////////////////////////////
//
//void medLabelsVisibilityParameter::setValues(const QHash<QString, bool> value)
//{
//    QHash<QString, QVariant>::const_iterator valuesIterator = value.constBegin();
//    bool valueUpdated = false;
//
//    while (valuesIterator != value.constEnd())
//    {
//        QString key = valuesIterator.key();
//        if (!d->variants.contains(key))
//        {
//            valuesIterator++;
//            continue;
//        }
//
//        if(d->variants[key].type() == QVariant::Double)
//        {
//            double previousValue = d->variants[key].toDouble();
//
//            if(valuesIterator.value().toDouble() < d->ranges.value(key).first.toDouble())
//                d->variants[key] = d->ranges.value(key).first;
//            else if(valuesIterator.value().toDouble() > d->ranges.value(key).second.toDouble())
//                d->variants[key] = d->ranges.value(key).second;
//            else
//                d->variants[key] = valuesIterator.value();
//
//            if( previousValue != d->variants[key] )
//                valueUpdated = true;
//        }
//        else if(d->variants[key].type() == QVariant::Int)
//        {
//            int previousValue = d->variants[key].toInt();
//
//            if(valuesIterator.value().toInt() < d->ranges.value(key).first.toInt())
//                d->variants[key] = d->ranges.value(key).first;
//            else if(valuesIterator.value().toInt() > d->ranges.value(key).second.toInt())
//                d->variants[key] = d->ranges.value(key).second;
//            else
//                d->variants[key] = valuesIterator.value();
//
//            if( previousValue != d->variants[key] )
//                valueUpdated = true;
//        }
//        else
//        {
//            d->variants[key] = valuesIterator.value();
//            valueUpdated = true;
//        }
//
//        valuesIterator++;
//    }
//
//    if(!valueUpdated)
//        return;
//
//    //  update intern widget
//    this->blockInternWidgetsSignals(true);
//    this->updateInternWigets();
//    this->blockInternWidgetsSignals(false);
//
//    emit valuesChanged(d->variants);
//}
//
//void medLabelsVisibilityParameter::updateInternWigets()
//{
//    QHash<QString, QVariant>::const_iterator i = d->variants.constBegin();
//    while (i != d->variants.constEnd())
//    {
//        QString name = i.key();
//        QVariant var = i.value();
//        QWidget* widget = d->widgets.value(name);
//
//        if(QCheckBox *checkbox = qobject_cast<QCheckBox*>(widget))
//            checkbox->setChecked(var.toBool());
//        else if(QSpinBox *spinBox = qobject_cast<QSpinBox*>(widget))
//            spinBox->setValue(var.toInt());
//        else if(QDoubleSpinBox *doubleSpinBox = qobject_cast<QDoubleSpinBox*>(widget))
//            doubleSpinBox->setValue(var.toDouble());
//
//        ++i;
//    }
//}
//
