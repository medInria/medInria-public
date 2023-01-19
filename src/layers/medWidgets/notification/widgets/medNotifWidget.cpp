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

#include "medNotifWidget.h"
#include "medNotifWindow.h"

#include <medNotif.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QDebug>


medNotifWidget::medNotifWidget(medUsrNotif &notif, medNotificationPaneWidget * paneContainer) : m_notif(notif)
{
    auto * widgetLayout = new QVBoxLayout;
    setLayout(widgetLayout);
    widgetLayout->setAlignment(Qt::AlignTop);

    if (paneContainer)
    {
        // Transparent background for round corners  
        setAttribute(Qt::WA_TranslucentBackground);
    }

    //--- Title area
    m_titleLayout = new QHBoxLayout;
    m_titleLayout->setAlignment(Qt::AlignTop);

    // Add criticality icons
    m_imageLabel = new QLabel(""); // QImage is not a widget, we need to add it in a QLabel
    m_imageLabel->setFixedSize(20, 20);
    m_imageLabel->setPixmap(criticalityImg(notif->getCriticalityLevel()));
    m_titleLayout->addWidget(m_imageLabel);

    // Add a title 
    m_titleLabel = new QLabel(notif->getTitle());
    m_titleLabel->setStyleSheet("font-weight: bold");
    m_titleLayout->addWidget(m_titleLabel);
    m_titleLayout->addStretch();

    // Add remove button
    m_removeNotifButton = new QPushButton();
    m_removeNotifButton->setIcon(QIcon(":/icons/cross.svg"));
    m_removeNotifButton->setIconSize(QSize(15, 15));
    m_removeNotifButton->setFixedSize(20, 20);
    m_titleLayout->addWidget(m_removeNotifButton);

    widgetLayout->addLayout(m_titleLayout);
    
    m_msgLabel = new QLabel(notif->getMessage());
    widgetLayout->addWidget(m_msgLabel);

    m_progressWidget = new QProgressBar();
    m_progressWidget->setMinimumSize(QSize(0, 0));
    m_progressWidget->setRange(0, 100);
    widgetLayout->addWidget(m_progressWidget);
    if (notif->getAchievement() > -1 && notif->getAchievement() <= 100)
    {
        m_progressWidget->setValue(notif->getAchievement());
        m_progressWidget->show();
    }
    else
    {
        m_progressWidget->hide();
    }

    m_extraLayout = new QGridLayout();
    widgetLayout->addLayout(m_extraLayout);
    m_extraTrigger1Button = nullptr;
    m_extraTrigger2Button = nullptr;
    m_extraTrigger3Button = nullptr;

    if (paneContainer)
    {
        connect(m_removeNotifButton, &QPushButton::clicked, [=]() {paneContainer->notifWidgetAskDeletion(this->m_notif); });
    }
    else
    {
        connect(m_removeNotifButton, &QPushButton::clicked, this, &medNotifWidget::deleteLater);
    }
    connect(notif.get(), &medNotif::updated, this, &medNotifWidget::update);
}

medNotifWidget::~medNotifWidget()
{
    int i = 0;
}

QSize medNotifWidget::minimumSizeHint() const
{
    return QSize(400, 100);
}

void medNotifWidget::update()
{
    m_imageLabel->setPixmap(criticalityImg(m_notif->getCriticalityLevel()));

    if (m_notif->getAchievement() > -1 && m_notif->getAchievement() <= 100)
    {
        m_progressWidget->setValue(m_notif->getAchievement());
        m_progressWidget->show();
    }
    else if (m_notif->getAchievement() == 101)
    {
        m_progressWidget->setRange(0, 0);
        m_progressWidget->setValue(0);
        m_progressWidget->show();
    }
    else
    {
        m_progressWidget->hide();
    }

    extraTriggerUpdate(1);
    extraTriggerUpdate(2);
    extraTriggerUpdate(3);
}


QPixmap & medNotifWidget::criticalityImg(notifLevel criticalityLevel)
{
    //These variables are static into a static member function to avoid reload multiple-times
    static QPixmap redSpot = QPixmap::fromImage(QIcon(":/icons/red_spot.svg").pixmap(QSize(15, 15)).toImage());
    static QPixmap yellowSpot = QPixmap::fromImage(QIcon(":/icons/yellow_spot.svg").pixmap(QSize(15, 15)).toImage());
    static QPixmap bleuSpot = QPixmap::fromImage(QIcon(":/icons/bleu_spot.svg").pixmap(QSize(15, 15)).toImage());
    static QPixmap greenSpot = QPixmap::fromImage(QIcon(":/icons/green_spot.svg").pixmap(QSize(15, 15)).toImage());

    switch (criticalityLevel)
    {
        case notifLevel::info:     return bleuSpot;
        case notifLevel::success:  return greenSpot;
        case notifLevel::warning: return yellowSpot;
        case notifLevel::error:    return redSpot;
        default: return redSpot;
    }
}

void medNotifWidget::extraTriggerUpdate(int extraCount)
{
    QString extraLabelText;
    QPushButton ** extraTriggerButton = nullptr;
    void(medNotif::*rebondSignal)() = nullptr;

    if (extraCount == 1)
    {
        extraLabelText = m_notif->getExtraTrigger1Label();
        extraTriggerButton = &m_extraTrigger1Button;
        rebondSignal = &medNotif::extraTrigger1;
    }
    else if (extraCount == 2)
    {
        extraLabelText = m_notif->getExtraTrigger2Label();
        extraTriggerButton = &m_extraTrigger2Button;
        rebondSignal = &medNotif::extraTrigger2;
    }
    else if (extraCount == 3)
    {
        extraLabelText = m_notif->getExtraTrigger2Label();
        extraTriggerButton = &m_extraTrigger2Button;
        rebondSignal = &medNotif::extraTrigger2;
    }

    if (!extraLabelText.isEmpty())
    {
        if (*extraTriggerButton == nullptr)
        {
            m_extraLayout->removeWidget(*extraTriggerButton);
            *extraTriggerButton = new QPushButton(extraLabelText);            
            m_extraLayout->addWidget(*extraTriggerButton, 0, extraCount);
            QObject::connect(*extraTriggerButton, &QPushButton::clicked, m_notif.get(), rebondSignal);
        }
        else
        {
            (*extraTriggerButton)->setText(extraLabelText);
        }
    }
    else
    {
        *extraTriggerButton = nullptr;
    }
}
