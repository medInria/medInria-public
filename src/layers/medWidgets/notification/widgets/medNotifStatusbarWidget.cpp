/*
 * medInria
 * Copyright (c) INRIA 2013. All rights reserved.
 * 
 * medInria is under BSD-2-Clause license. See LICENSE.txt for details in the root of the sources or:
 * https://github.com/medInria/medInria-public/blob/master/LICENSE.txt
 * 
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "medNotifStatusbarWidget.h"
#include "medNotifStatusbarSubPart.h"

#include <medNotif.h>
#include <medNotifSys.h>

#include <QMap>
#include <QTimer>
#include <QMutex>
#include <QMutexLocker>

#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QMenu>
#include <QWidgetAction>


class medNotifStatusbarItemPrivate
{
public:
    medNotifSys * notificationSystem; 
    QMutex mutex;
    
    QMap<medNotif*, medNotifStatusbarSubpart*> notifToWidgetMap;
    QMap<medNotif*, QWidgetAction*> notifToWidgetActionMap;
    medNotifStatusbarSubpart* displayed[3];
    QList<medNotifStatusbarSubpart*> fifoNotifSubWidget;

    //QGridLayout * layout;
    QHBoxLayout * mainLayout;
    QHBoxLayout * notifslayout;
    QWidget * notifsWidget;
    QPushButton * showExtra;
    QMenu *extraList;
};



medNotifStatusbarWidget::medNotifStatusbarWidget(medNotifSys * notificationSystem)
{
    d = new medNotifStatusbarItemPrivate();
    d->notificationSystem = notificationSystem;
    //d->layout = new QGridLayout(this);
    d->mainLayout = new QHBoxLayout(this);
    d->mainLayout->setMargin(0);
    d->notifsWidget = new QWidget(this);
    d->notifslayout = new QHBoxLayout();
    d->notifslayout->setMargin(0);
    d->notifsWidget->setLayout(d->notifslayout);
    d->notifsWidget->setContentsMargins(0, 0, 0, 0); 
    d->showExtra = new QPushButton("...", this);
    d->showExtra->setFixedWidth(33);
    d->showExtra->setEnabled(false);
    d->extraList = new QMenu(this);
    d->showExtra->setMenu(d->extraList);
    this->setFixedWidth(900);

    setLayout(d->mainLayout);
    //d->layout->addWidget(d->showExtra, 0, 3, Qt::AlignCenter);
    d->mainLayout->addWidget(d->notifsWidget);//, 1, Qt::AlignLeft);
    d->mainLayout->addWidget(d->showExtra);//, 0, Qt::AlignRight);

    d->displayed[0] = nullptr;
    d->displayed[1] = nullptr;
    d->displayed[2] = nullptr;

    this->setContentsMargins(0, 0, 0, 0);
    this->setStyleSheet("border: 2px solid red; border-radius: 10px;");

    auto ok = connect(notificationSystem, &medNotifSys::removed, this, &medNotifStatusbarWidget::removeNotification);
         ok = connect(notificationSystem, &medNotifSys::update, this, &medNotifStatusbarWidget::update);

}

medNotifStatusbarWidget::~medNotifStatusbarWidget()
{
    delete d;
}


void medNotifStatusbarWidget::createSubPartWidget(medNotif * notif)
{    
    if (notif->getAchievement() > -1)
    {
        auto * notifTBWidget = new medNotifStatusbarSubpart(/*d->notifsWidget*/nullptr);


        notifTBWidget->m_titleWidget->setText(notif->getTitle());
        if (notif->getAchievement() == 101)
        {
            notifTBWidget->m_progressBar->setRange(0, 0);
            notifTBWidget->m_progressBar->setValue(0);
        }
        d->notifToWidgetMap[notif] = notifTBWidget;
        if (d->displayed[0] == nullptr)
        {
            d->displayed[0] = notifTBWidget;
            //d->layout->addWidget(d->displayed[0], 0, 0, Qt::AlignLeft);
            d->notifslayout->addWidget(d->displayed[0]);
            //notifTBWidget->setParent(d->notifsWidget);
            
        }
        else if (d->displayed[1] == nullptr)
        {
            d->displayed[1] = notifTBWidget;
            //d->layout->addWidget(d->displayed[1], 0, 1, Qt::AlignLeft);
            d->notifslayout->addWidget(d->displayed[1]);
            //notifTBWidget->setParent(d->notifsWidget);
        }
        else if (d->displayed[2] == nullptr)
        {
            d->displayed[2] = notifTBWidget;
            //d->layout->addWidget(d->displayed[2], 0, 2, Qt::AlignLeft);
            d->notifslayout->addWidget(d->displayed[2]);
            //notifTBWidget->setParent(d->notifsWidget);
        }
        else
        {
            auto pAction = new QWidgetAction(this);
            d->fifoNotifSubWidget << notifTBWidget;
            d->notifToWidgetActionMap[notif] = pAction;
            pAction->setDefaultWidget(notifTBWidget);
            notifTBWidget->setParent(d->extraList);
            d->extraList->addAction(pAction);
            d->showExtra->show();
        }
        connect(notifTBWidget, &medNotifStatusbarSubpart::clicked, [=]() {emit this->notifClicked(notif); });
       
        notifTBWidget->show();
        
        if (!d->fifoNotifSubWidget.isEmpty())
        {
            d->showExtra->setEnabled(true);
        }
    }
}

void medNotifStatusbarWidget::removeNotification(medNotif * notif)
{
    QMutexLocker(&d->mutex);
    if (d->notifToWidgetMap.contains(notif))
    {
        auto *pWidget = d->notifToWidgetMap.take(notif);
        int pos = -1;

        if (d->displayed[0] == pWidget) pos = 0;
        else  if (d->displayed[1] == pWidget) pos = 1;
        else  if (d->displayed[2] == pWidget) pos = 2;
        
        if (pos > -1)
        {
            //hide widgets and move they
            for (int i = pos; i < 3; ++i)
            {
                medNotifStatusbarSubpart * righterWidget = nullptr;
                i < 2 ? righterWidget = d->displayed[i + 1] : righterWidget = nullptr;

                if (d->displayed[i])
                {
                    d->displayed[i]->hide();
                }
                d->displayed[i] = righterWidget;
            }

            //remove widget associated to notif
            d->notifslayout->removeWidget(pWidget);

            //get a widget from fifo and remove this one from menu
            if (!d->fifoNotifSubWidget.isEmpty())
            {
                auto *pWidgetFromList = d->fifoNotifSubWidget.takeFirst();

                d->displayed[2] = pWidgetFromList;
                //d->layout->addWidget(d->displayed[2], 0, 2, Qt::AlignLeft);
                d->notifslayout->addWidget(d->displayed[2]);
                d->displayed[2]->hide();
                d->displayed[2]->setParent(d->notifsWidget);
                auto * pActionFromList = d->notifToWidgetActionMap.take(d->notifToWidgetMap.key(pWidgetFromList));
                pActionFromList->setDefaultWidget(new QWidget());
                d->extraList->removeAction(pActionFromList);
            }

            //Show widgets
            for (int i = 0; i < 3; ++i)
            {
                //d->layout->addWidget(d->displayed[i], 0, i, Qt::AlignLeft);
                d->notifslayout->addWidget(d->displayed[i]);
                if (d->displayed[i])
                {
                    d->displayed[i]->show();
                }
            }
        }
        else
        {
            auto * pActionFromList = d->notifToWidgetActionMap.take(notif);
            d->extraList->removeAction(pActionFromList);
        }
    }

    if (d->fifoNotifSubWidget.isEmpty())
    {
        d->showExtra->setEnabled(false);
    }
}


void medNotifStatusbarWidget::update(medNotif * notif)
{
    if (notif)
    {
        QMutexLocker(&d->mutex);
        if (!d->notifToWidgetMap.contains(notif))
        {
            createSubPartWidget(notif);
            //d->notifToWidgetMap[notif]->setFixedWidth(100);
            //d->notifToWidgetMap[notif]->setFixedHeight(100);
            //d->notifToWidgetMap[notif]->show();
            //this->show();
        }
        int percentage = notif->getAchievement();
        if (notif->getCriticalityLevel() == notifLevel::info)
        {
            //d->notifToWidgetMap[notif]->m_progressBar->setValue(percentage);
        }

        if(notif->getCriticalityLevel() != notifLevel::info || percentage == 100)
        {
            QTimer::singleShot(50000, [=]()
            {this->removeNotification(notif); }
            );
        }
    }
}
