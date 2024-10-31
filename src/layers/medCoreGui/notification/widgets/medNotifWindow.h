#pragma once
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

#include <QWidget>
#include <QMap>
#include <QListWidget>

#include <medCoreGuiExport.h>
#include <medNotifSys.h>

class QListWidgetItem;
class QPropertyAnimation;
class QMainWindow;

class MEDCOREGUI_EXPORT medNotificationPaneWidget : public QWidget
{
    Q_OBJECT

public:
    medNotificationPaneWidget(medNotifSys * pi_pNotification, QMainWindow *pi_parent = nullptr);
    ~medNotificationPaneWidget() = default;

    void setParent(QMainWindow *pi_parent);
    void paintEvent(QPaintEvent *event);

public slots:
    void notifWidgetAskDeletion(medUsrNotif notif);
    void removeNotification(medUsrNotif notif);
    void addNotification(medUsrNotif notif);
    void showPane(bool show);
    void windowGeometryUpdate();
    void showAndHigligth(medUsrNotif notif);
    void switchVisibility();
    void clicked(QPoint);

signals:
    void expanded(bool);

protected:

private:
    //Data
    medNotifSys                       * m_notificationSystem; /*!< pointer on notification system. */
    QMap<medNotif*, QListWidgetItem*>   m_notificationToItem; /*!< Maps of items used by m_notificationsListWidget stored by associated medNotif pointer. */
    QListWidget                       * m_notificationsListWidget; /*!< Widget list of notifications instances. */
    QPropertyAnimation                * m_animation;
    QPropertyAnimation                * m_geoAnimation;
    QPropertyAnimation                * m_alphaAnimation;
    QMainWindow                       * m_parent;
    QSize                               m_winSize;
    bool                                isDisplayed;
};