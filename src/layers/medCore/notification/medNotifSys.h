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

Q_DECLARE_METATYPE(medUsrNotif);

class MEDCORE_EXPORT medNotifSys : public QObject
{
    Q_OBJECT

public:
    static medNotifSys* instance();

    static bool registerNotif  (medUsrNotif &notif);
    static bool unregisterNotif(medUsrNotif const & notif);

    void setOperatingSystemNotification(bool pi_bOn);
    void setOSNotifOnlyNonFocus        (bool pi_bOn);

signals:
    void update       (medUsrNotif);
    void notification (medUsrNotif);
    void removed      (medUsrNotif);

public slots:
    bool add(medUsrNotif notif);
    bool remove2(medUsrNotif notif);
    void windowOnTop(bool pi_bOntop);
    void clear();

private:    
    medNotifSys(QObject *parent = nullptr);
    ~medNotifSys();

    medUsrNotif getUsrNotif(medNotif * notif);
    bool remove();
    void updateNotif();
    void osNotif(medUsrNotif &notif);

private:
    friend medNotifSysPrivate;
    QScopedPointer<medNotifSysPrivate> d;
};
