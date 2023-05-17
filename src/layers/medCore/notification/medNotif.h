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
#include <QString>

#include <memory>

#include <medCoreExport.h>

class medNotifPrivate;
class medNotifSys;

enum class notifLevel : char
{
    info,
    success,
    warning,
    error
};

class MEDCORE_EXPORT medNotif : public QObject
{
    Q_OBJECT

public:
    static std::shared_ptr<medNotif> createNotif(notifLevel pi_criticityLevel, QString pi_title, QString pi_message, int pi_time_ms = -1, int pi_achievementPercentage = -1);
    static std::shared_ptr<medNotif> createNotif(medNotifSys * sys, notifLevel pi_criticityLevel, QString pi_title, QString pi_message, int pi_time_ms = -1, int pi_achievementPercentage = -1);

    ~medNotif();
    bool update(notifLevel pi_criticityLevel, int pi_achievementPercentage, QString newMessage = "");
    
    notifLevel getCriticalityLevel();
    QString getTitle();
    QString getMessage();
    int getAchievement();
    int getDisplayTime();

    void setExtraTrigger1Label(QString const & label);
    void setExtraTrigger2Label(QString const & label);
    void setExtraTrigger3Label(QString const & label);
    QString getExtraTrigger1Label();
    QString getExtraTrigger2Label();
    QString getExtraTrigger3Label();

    QString toString();

signals:
    void updated();
    void extraTrigger1();
    void extraTrigger2();
    void extraTrigger3();

private:
    medNotif();
    medNotifPrivate * d;
};
