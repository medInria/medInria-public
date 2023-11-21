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

#include <QFrame>

#include <medNotif.h>
#include <medNotifSys.h>



class medNotificationPaneWidget;

class QListWidgetItem;
class QHBoxLayout;
class QLabel;
class QPushButton;
class QProgressBar;
class QGridLayout;


class medNotifWidget : public QFrame
{
    Q_OBJECT

public:
    medNotifWidget(medUsrNotif &notif, medNotificationPaneWidget *paneContainer);
    ~medNotifWidget();

    QSize minimumSizeHint() const override;

public slots:
    void update();
    void closeButton();

private:
    void extraTriggerUpdate(int extraCount);
    static QPixmap & criticalityImg(notifLevel criticalityLevel);

private:
    medNotificationPaneWidget *m_pPane;
    medUsrNotif    m_notif;
    QHBoxLayout  * m_titleLayout;
    QLabel       * m_titleLabel;

    QLabel       * m_imageLabel;
    QPushButton  * m_removeNotifButton;

    QLabel       * m_msgLabel;
    QProgressBar * m_progressWidget;

    QGridLayout  * m_extraLayout;
    QPushButton  * m_extraTrigger1Button;
    QPushButton  * m_extraTrigger2Button;
    QPushButton  * m_extraTrigger3Button;

    QSize          m_sourceWidgetSize;
};