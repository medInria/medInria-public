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

#include <medWidgetsExport.h>

#include <medNotifSys.h>
class medNotif;

class medNotifStatusbarItemPrivate;
class medNotifStatusbarWidget : public QWidget
{
    Q_OBJECT
public:
    medNotifStatusbarWidget(medNotifSys * notificationSystem);
    ~medNotifStatusbarWidget();

public slots:
    void removeNotification (medUsrNotif notif);
    void createSubPartWidget(medUsrNotif notif);
    void update             (medUsrNotif notif);

signals:
    void notifClicked(medUsrNotif);

private:
    medNotifStatusbarItemPrivate *d;
};

