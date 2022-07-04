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

#include <medCoreExport.h>

class medNotificationPrivate;
class MEDCORE_EXPORT medNotification : public QObject
{
    Q_OBJECT

public:
    enum class notifLevel : char
    {
        info,
        success,
        warnning,
        error
    };

    medNotification(QObject *parent);
    ~medNotification();


    static int notify (notifLevel pi_level, QString pi_title, QString pi_message, int pi_time_ms = -1, int pi_achivementPercentage = -1, int pi_notifId = 0);
     
    static int  infoWithProgress (QString pi_title, QString pi_message, int pi_time_ms = -1, int pi_achivementPercentage = -1, int pi_notifId = 0);
    static void info             (QString pi_title, QString pi_message, int pi_time_ms = -1);
    static void success          (QString pi_title, QString pi_message, int pi_time_ms = -1);
    static void warning          (QString pi_title, QString pi_message, int pi_time_ms = -1);
    static void error            (QString pi_title, QString pi_message);

signals:
    void notification(QJsonObject const&);

public slots:
    void remove(int pi_id);

private:
    int notify_internal(notifLevel pi_level, QString pi_title, QString pi_message, int pi_time_ms = -1, int pi_achivementPercentage = -1, int pi_notifId = 0);

private:
    QScopedPointer<medNotificationPrivate> d;
};

