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

#include "medNotifWindow.h"

#include <medNotifSys.h>
#include <medNotif.h>

#include <medNotifWidget.h>

#include <algorithm>

#include <QPropertyAnimation>
#include <QPauseAnimation>
#include <QSequentialAnimationGroup>
#include <QMainWindow>
#include <QStatusBar>

#include <QPushButton>
#include <QGraphicsOpacityEffect>
#include <QVBoxLayout>

medNotificationPaneWidget::medNotificationPaneWidget(medNotifSys * pi_pNotifSys, QMainWindow * pi_parent) : QWidget(pi_parent)
{    
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    setLayout(layout);

    m_notificationsListWidget = new QListWidget(this);
    layout->addWidget(m_notificationsListWidget);

    m_notificationsListWidget->setAcceptDrops(false);
    m_notificationsListWidget->setAutoScroll(true);
    m_notificationsListWidget->setResizeMode(QListView::Adjust);
    m_notificationsListWidget->setStyleSheet("QListWidget::item:selected {background:gray;}");
    m_animation = new QPropertyAnimation(this, "geometry", this);

    connect(pi_pNotifSys, &medNotifSys::notification, this, &medNotificationPaneWidget::addNotification);
    connect(pi_pNotifSys, &medNotifSys::removed, this, &medNotificationPaneWidget::removeNotification);
    setParent(pi_parent);
    showPane(false);

    m_geoAnimation = nullptr;
}

void medNotificationPaneWidget::notifWidgetAskDeletion(medNotif *notif)
{
    m_notificationSystem->unregisterNotif(notif); //TODO this is static method, stupid call
}

void medNotificationPaneWidget::setParent(QMainWindow * pi_parent)
{
    QWidget::setParent(pi_parent);
    m_parent = pi_parent;
    if (m_parent)
    {
        m_winSize = m_parent->size();
    }
}

void medNotificationPaneWidget::removeNotification(medNotif * notif)
{
    auto *pItem = m_notificationToItem.take(notif);
    m_notificationsListWidget->removeItemWidget(pItem);
    delete pItem;
}

void medNotificationPaneWidget::addNotification(medNotif * notif)
{
    QListWidgetItem *widgetItem = new QListWidgetItem();
    medNotifWidget *newNotifiWidget = new medNotifWidget(notif, this);
    m_notificationsListWidget->addItem(widgetItem);
    m_notificationsListWidget->setItemWidget(widgetItem, newNotifiWidget);
    m_notificationToItem[notif] = widgetItem;

    // Initial size of item
    QSize initialSize = newNotifiWidget->size();
    widgetItem->setSizeHint(initialSize);

    m_notificationsListWidget->setSpacing(2);



    medNotifWidget *newNotifiWidgetPopup = new medNotifWidget(notif, nullptr);

    newNotifiWidgetPopup->setFixedSize(QSize(400, 75));
    newNotifiWidgetPopup->move(QPoint(10, 10));
    newNotifiWidgetPopup->setParent(m_parent);
    newNotifiWidgetPopup->show();

    QAnimationGroup * animationGroup = new QSequentialAnimationGroup(newNotifiWidgetPopup);


    int duration_ms = 500;
    m_geoAnimation = new QPropertyAnimation(newNotifiWidgetPopup, "pos");
    QPoint point_A((m_winSize.width()/2) - 200, -75);
    QPoint point_B((m_winSize.width()/2) - 200 , 0);
    m_geoAnimation->setDuration(duration_ms);
    m_geoAnimation->setStartValue(point_A);
    m_geoAnimation->setEndValue(point_B);

    QPauseAnimation *pauseAnimation = new QPauseAnimation(3000, this);

    QGraphicsOpacityEffect * show_effect = new QGraphicsOpacityEffect(newNotifiWidgetPopup);
    show_effect->setOpacity(1);
    m_alphaAnimation = new QPropertyAnimation(show_effect, "opacity");
    newNotifiWidgetPopup->setGraphicsEffect(show_effect);
    m_alphaAnimation->setStartValue(1);
    m_alphaAnimation->setEndValue(0);
    m_alphaAnimation->setDuration(duration_ms);


    animationGroup->addAnimation(m_geoAnimation);
    animationGroup->addAnimation(pauseAnimation);
    animationGroup->addAnimation(m_alphaAnimation);
    
    animationGroup->start();
}

void medNotificationPaneWidget::showPane(bool show)
{
    auto paneWidth = std::max(300, m_winSize.width() / 4);
    int statusBarHeight = 0;
    if (m_parent)
    {
        statusBarHeight = m_parent->statusBar()->height();
    }
    QRect rect_A(m_winSize.width(), 0, 0, m_winSize.height() - statusBarHeight);
    QRect rect_B(m_winSize.width() - paneWidth, 0, paneWidth, m_winSize.height() - statusBarHeight);
    if (show)
    {
        m_animation->setStartValue(rect_A);
        m_animation->setEndValue(rect_B);
    }
    else
    {
        m_animation->setStartValue(rect_B);
        m_animation->setEndValue(rect_A);
    }
    m_animation->start();
}

void medNotificationPaneWidget::windowGeometryUpdate(QRect const & geo)
{
    m_winSize = geo.size();
    if (width())
    {
        int statusBarHeight = 0;
        if (m_parent)
        {
            statusBarHeight = m_parent->statusBar()->height();
        }
        auto paneWidth = std::max(300, m_winSize.width() / 4);
        QRect rect_B(m_winSize.width() - paneWidth, 0, paneWidth, m_winSize.height() - statusBarHeight);
        setGeometry(rect_B);
    }
}

void medNotificationPaneWidget::showAndHigligth(medNotif * notif)
{
    if (m_notificationToItem.contains(notif))
    {
        showPane(true);
        m_notificationsListWidget->setCurrentItem(m_notificationToItem[notif]);
    }
}

void medNotificationPaneWidget::swithVisibility()
{
    showPane(!(bool)width());
}

