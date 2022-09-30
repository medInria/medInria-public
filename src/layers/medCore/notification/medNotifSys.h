#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QObject>
#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>

#include <memory>

#include <medCoreExport.h>

class medNotifSysPrivate;
class medNotif;
using medUsrNotif = std::shared_ptr<medNotif>;


class MEDCORE_EXPORT medNotifSys : public QObject
{
    Q_OBJECT

public:
    static medNotifSys* instance();
    static bool registerNotif(medUsrNotif notif);
    static bool unregisterNotif(medNotif *notif);
    static void updateNotif(medNotif *notif);

    void setOperatingSystemNotification(bool pi_bOn);
    void setOSNotifOnlyNonFocus(bool pi_bOn);

signals:
    void update(medNotif *);
    void notification(medNotif *);
    void removed(medNotif *);

public slots:
    void remove(medNotif * pi_pNotif);
    void windowOnTop(bool pi_bOntop);

private:    
    medNotifSys(QObject *parent = nullptr);
    ~medNotifSys();
    void osNotif(medNotif *notif);
    void debugWriteNotifications(medNotif *notif);//TODO remove after debug phase

private:
    friend medNotifSysPrivate;
    QScopedPointer<medNotifSysPrivate> d;
};
