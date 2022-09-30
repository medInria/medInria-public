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

class medNotifSys;
class medNotif;

class medNotifStatusbarItemPrivate;
class medNotifStatusbarWidget : public QWidget
{
    Q_OBJECT
public:
    medNotifStatusbarWidget(medNotifSys * notificationSystem);
    ~medNotifStatusbarWidget();

public slots:
    void removeNotification(medNotif * notif);
    void createSubPartWidget(medNotif * notif);
    void update(medNotif * notif);

signals:
    void notifClicked(medNotif *);

private:
    medNotifStatusbarItemPrivate *d;
};

